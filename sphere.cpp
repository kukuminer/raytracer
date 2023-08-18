#pragma once
#include <string>
#include "glm/vec4.hpp"
#include "glm/glm.hpp"
// #include "glm/mat4x4.hpp"

class sphere
{
public:
	std::string name;
	float x, y, z, sx, sy, sz;
	float red, green, blue;
	float a, d, s, r; //ambient, diffuse, spec, reflective
	int spec;
	glm::vec4 pos, colour;

	glm::mat4 trm;
	glm::mat4 inv;
	glm::mat4 invT;

	// Empty ctor because fields are public (set after initialization for easy input)
	sphere()
	{}

	sphere(std::string iname, float ix, float iy, float iz, float isx, float isy, float isz,
		float ired, float igreen, float iblue,
		float ia, float id, float is, float ir, int ispec)
	{
		init(iname, ix, iy, iz, isx, isy, isz, ired, igreen, iblue, ia, id, is, ir, ispec);
	}

	void init(std::string iname, float ix, float iy, float iz, float isx, float isy, 
		float ired, float igreen, float iblue,
		float isz, float ia, float id, float is, float ir, int ispec)
	{
		name = iname;
		x = ix;
		y = iy;
		z = iz;
		sx = isx;
		sy = isy;
		sz = isz;
		red = ired;
		green = igreen;
		blue = iblue;
		a = ia;
		d = id;
		s = is;
		r = ir;
		spec = ispec;
		init();
	}

	void init()
	{
		initVecs();
		initMats();
	}

	void initVecs()
	{
		this->pos = glm::vec4(x, y, z, 1);
		this->colour = glm::vec4(red, green, blue, 1.0f);
	}

	void initMats()
	{
		glm::mat4 trans = glm::mat4({
			{1, 0, 0, x},
			{0, 1, 0, y},
			{0, 0, 1, z},
			{0, 0, 0, 1}
		});
		glm::mat4 scale = glm::mat4({
			{sx, 0, 0, 0},
			{0, sy, 0, 0},
			{0, 0, sz, 0},
			{0, 0, 0, 1}
		});
		trm = scale * trans; // glm matrix/vector multiplication is right to left
		inv = glm::inverse(trm);
		invT = glm::transpose(inv);
	}

	void print()
	{
		printf("x: %.2f, y: %.2f, z: %.2f, sx: %.2f, sy: %.2f, sz: %.2f RGB: [%.2f %.2f %.2f]\n",
			 x, y, z, sx, sy, sz, red, green, blue);
	}
};