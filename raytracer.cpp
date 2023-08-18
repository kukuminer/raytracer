#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "glm/glm.hpp"
#include "glm/matrix.hpp"
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "sphere.cpp"
#include "light.cpp"
#include "ppm.cpp"
using namespace std;
using namespace glm;

#define RAY_EPSILON 0.0001
#define PI 3.1415926535
#define EYE vec4(0, 0, 0, 1)
#define MAX_BOUNCES 3

class raytracer
{
public:
	double near, left, right, bottom, top;
	int cols, rows;
	vector<sphere*> spheres;
	vector<light*> lights;
	vec3 background, ambient;
	string output;

	//ctor
	raytracer(string file)
	{
		string line;
		ifstream in(file);
		while(getline(in, line))
		{
			string word;
			istringstream ss(line);
			// cout << line << endl;

			ss >> word;
			if(word == "NEAR") ss >> near;
			if(word == "LEFT") ss >> left;
			if(word == "RIGHT") ss >> right;
			if(word == "BOTTOM") ss >> bottom;
			if(word == "TOP") ss >> top;
			if(word == "RES") ss >> cols >> rows;
			if(word == "SPHERE")
			{
				// string name;
				// float x, y, z, sx, sy, sz;
				// float a, d, s, r; //ambient, diffuse, spec, reflective
				// int spec;
				sphere* s = new sphere();
				ss >> s->name >> s->x >> s->y >> s->z >> s->sx >> s->sy >> s->sz >> s->red >> s->green >> s->blue >> s->a >> s->d >> s->s >> s->r >> s->spec;
				s->init();
				spheres.push_back(s);
			}
			if(word == "LIGHT")
			{
				// string name;
				// float x, y, z, r, g, b;
				light* l = new light();
				ss >> l->name >> l->x >> l->y >> l->z >> l->r >> l->g >> l->b;
				l->initVecs();
				lights.push_back(l);
			}
			if(word == "BACK") ss >> background[0] >> background[1] >> background[2];
			if(word == "AMBIENT") ss >> ambient[0] >> ambient[1] >> ambient[2];
			if(word == "OUTPUT") ss >> output;
		}
		cout << "----------------------------------" << endl;
		cout << "Input complete for " << output << endl;
		cout << "-" << endl;
		// print();
	}

	void print() // Prints properties
	{
		printf("-------------\n");
		printf("Raytracer for %s\n", output.c_str());
		printf("NEAR: %.2f, LEFT: %.2f, RIGHT: %.2f, BOTTOM: %.2f, TOP: %.2f\n", near, left, right, bottom, top);
		printf("RES: %i x %i\n", cols, rows);
		printf("SPHERES:\n");
		for(auto s : spheres)
		{
			cout << '\t' << "SPHERE ";
			s->print();
		}
		printf("LIGHTS:\n");
		for(auto l : lights)
		{
			cout << '\t' << "LIGHT ";
			l->print();
		}
		printf("BG: [%.2f, %.2f, %.2f]\n", background[0], background[1], background[2]);
		printf("AMB: [%.2f, %.2f, %.2f]\n", ambient[0], ambient[1], ambient[2]);
		printf("-------------\n");

	}

	vector<vec4> getPixels() // Gets all the pixel positions on the near plane
	{
		vector<vec4> pix;
		for(int a = 0; a < rows; a++)
		{
			for(int b = 0; b < cols; b++)
			{
				float z = -near;
				float x = mix(left, right, (b+0.5) / cols);
				float y = mix(top, bottom, (a+0.5) / cols);
				pix.push_back(vec4(x, y, z, 1));
			}
		}
		return pix;
	}

	vector<vec4> getRays() // Gets all initial rays from EYE to each pixel
	{
		vector<vec4> rays;
		vector<vec4> pixels = getPixels();
		for(auto p : pixels)
		{
			rays.push_back(cast(EYE, p));
		}
		return rays;
	}

	vector<vector<vec3>> getImage() // Gets a 2d array of pixel colours
	{
		vector<vec4> rays = getRays();
		vector<vector<vec3>> img(rows, vector<vec3>(cols, vec3(255.0f * background)));
		for(int r = 0; r < rays.size(); r++)
		{
			vec3 pix = colourPixel(EYE, rays[r], 0);
			// Clamp pixel colours to [0, 255]
			img[r / rows][r % cols] = clamp(pix*255.0f, vec3(0), vec3(255));
		}
		return img;
	}

	vec3 colourPixel(vec4 isrc, vec4 iray, int bounceNum)
	{
		vec4 inter = intersectRaySphere(isrc, iray, spheres);

		// If the ray doesn't collide with anything, return background colour or black
		if(inter[3] == -1) 
		{
			if(bounceNum == 0) return background;
			else return vec3(0, 0, 0);
		}
		return (
			ambientLight(inter) + 
			diffuseLight(isrc, iray, inter) + 
			specularLight(isrc, iray, inter) + 
			reflectedLight(isrc, iray, inter, bounceNum)
		);
	}

	vec3 ambientLight(vec4 inter)
	{
		sphere* s = spheres[inter[3]];
		return vec3(s->colour) * s->a * ambient;
	}

	vec3 diffuseLight(vec4 isrc, vec4 iray, vec4 inter)
	{
		// the 4th # of the intersection point contains the index of the sphere
		sphere* s = spheres[inter[3]];
		vec3 colourSum = vec3(0, 0, 0);
		if(s->d > 0)
		{
			// inter[3] contains sphere index. Replace it with 1
			inter[3] = 1;
			for(auto l : lights)
			{
				vec3 toLight = vec3(l->pos - inter);
				vec4 pathToLight = intersectRaySphere(inter, vec4(toLight, 0), spheres);
				// If no intersections, 4th # is -1
				float LtoInt = pathToLight[3] != -1 ? length(vec3(pathToLight) - vec3(inter)) : INFINITY;
				float LtoL = length(toLight);
				if(pathToLight[3] == -1 || LtoInt > LtoL)
				{
					vec3 norm = vec3(inter - s->pos);
					// Multiply by inverse matrix of sphere to set norm to canonical sphere
					// Multiply again to make it match the actual sphere shape (as mentioned in slides)
					norm = norm * mat3(s->invT) * mat3(s->inv);
					norm = normalize(norm);
					
					// If the incident ray and normal are within 90 degrees, we are inside the sphere
					// Flip normal if this occurs
					if(dot(normalize(vec3(iray)), norm) > 0) norm = -norm;
					toLight = normalize(toLight);
					float factor = std::max(dot(toLight, norm), 0.0f);
					colourSum += vec3(l->colour * s->colour * s->d * factor);
				}
			}
		}
		return colourSum;
	}

	vec3 specularLight(vec4 isrc, vec4 iray, vec4 inter)
	{
		sphere* s = spheres[inter[3]];
		vec3 colourSum = vec3(0, 0, 0);
		if(s->s > 0)
		{
			// inter[3] contains sphere index. Replace it with 1
			inter[3] = 1;
			for(auto l : lights)
			{
				vec3 toLight = vec3(l->pos - inter);
				// To check if the path to the light is clear:
				vec4 pathToLight = intersectRaySphere(inter, vec4(toLight, 0), spheres);
				
				float LtoInt = pathToLight[3] != -1 ? length(vec3(pathToLight) - vec3(inter)) : INFINITY;
				float LtoL = length(toLight);
				// If no intersection, 4th # is -1
				// If the intersection is past the light source, ignore it (it would not cast shadow)
				if(pathToLight[3] == -1 || LtoInt > LtoL)
				{
					vec3 norm = vec3(inter - s->pos);
					norm = norm * mat3(s->invT) * mat3(s->inv);
					norm = normalize(norm);
					vec3 ref = reflect(normalize(vec3(iray)), norm);
					ref = normalize(ref);
					toLight = normalize(toLight);
					float factor = pow(std::max(dot(toLight, ref), 0.0f), s->spec);
					colourSum += vec3(l->colour * s->s * factor);
				}
			}
		}
		return colourSum;
	}

	vec3 reflectedLight(vec4 isrc, vec4 iray, vec4 inter, int bounceNum) //Calc reflect light
	{
		sphere* s = spheres[inter[3]];
		vec3 colourSum = vec3(0, 0, 0);
		// If we still have bounces, and a reflective coefficient, raytrace from this point again!
		if (s->r > 0 && bounceNum < MAX_BOUNCES)
		{
			vec3 norm = vec3(inter - s->pos);
			norm = norm * mat3(s->invT * s->invT);
			norm = normalize(norm);
			vec3 ref = reflect(normalize(vec3(iray)), norm);
			ref = normalize(ref);

			colourSum += s->r * colourPixel(vec4(vec3(inter), 1), vec4(ref, 0), bounceNum + 1);
		}
		return colourSum;
	}

	void makeImg() // Get reqs for save_imageP6
	{
		vector<vector<vec3>> clrs = getImage();
		vector<unsigned char> imgVec;
		char* output2 = const_cast<char*>(output.c_str());
		int index = 0;
		for(int a = 0; a < rows; a++)
		{
			for(int b = 0; b < cols; b++)
			{
				for(int c = 0; c < 3; c++)
				{
					imgVec.push_back(clrs[a][b][c]);
					index++;
				}
			}
		}
		unsigned char* img = reinterpret_cast<unsigned char*>(imgVec.data());
		save_imageP6(cols, rows, output2, img);
	}

	static vec4 cast(vec4 p1, vec4 p2) // Make ray from p1 to p2
	{
		vec4 v = p2 - p1;
		// v = normalize(v);
		return v;
	}

	vec4 intersectRaySphere(vec4 isrc, vec4 iray, vector<sphere*> spheres)
	{
		float minT = INFINITY;
		int sphereIndex = -1;
		bool origin = (isrc[2] == EYE[2]);
		for(int i = 0; i < spheres.size(); i++)
		{
			sphere* s = spheres[i];

			vector<vec4> ans;
			// Invert the transformation on the sphere & apply the same to the ray

			vec3 src = vec3(isrc * s->inv);
			vec3 ray = vec3(iray * s->inv);

			// line = src + t*ray where t = real number
			// radius = 1
			// sphere = |P| - 1 = 0
			// plug line into P
			// => |ray|^2 * t^2  +  2(src * ray) * t  +  |src|^2-1  =  0
			//    {  a  }            {    b    }         {   c   }
			float a = dot(ray, ray);
			float b = dot(ray, src);
			float c = dot(src, src) - 1;
			float det = b*b - a*c;

			// t = -b / a +- (rt(b*b - a*c) / a)
			float t1 = (-b / a) + (sqrt(det)/a);
			float t2 = (-b / a) - (sqrt(det)/a);
			if(det >= 0)
			{
				// Needs to be in "front" of the src point, in case src is a collision or behind the camera
				// If origin is eye, also needs to be in front of near clipping plane
				if(t1 > RAY_EPSILON + origin && t1 < minT)
				{
					minT = t1;
					sphereIndex = i;
				}
				if(t2 > RAY_EPSILON + origin && t2 < minT)
				{
					minT = t2;
					sphereIndex = i;
				}
			}
		}
		if(minT == INFINITY) return vec4(0, 0, 0, -1);
		vec3 colPt = isrc + minT * iray;
		return vec4(colPt, sphereIndex);
	}
};
