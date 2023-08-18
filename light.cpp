#pragma once
#include <string>
#include "glm/vec4.hpp"

class light
{
public:
	std::string name;
	float x, y, z, r, g, b;
	glm::vec4 pos, colour;

	light(){}
	light(std::string iname, float ix, float iy, float iz, float ir, float ig, float ib)
	{
		init(iname, ix, iy, iz, ir, ig, ib);
	}

	void init(std::string iname, float ix, float iy, float iz, float ir, float ig, float ib)
	{
		name = iname;
		x = ix;
		y = iy;
		z = iz;
		r = ir;
		g = ig;
		b = ib;
		initVecs();
	}

	void initVecs()
	{
		this->pos = glm::vec4(x, y, z, 1);
		this->colour = glm::vec4(r, g, b, 1.0f);
	}

	void print()
	{
		printf("x: %.2f, y: %.2f, z: %.2f, r: %.2f, g: %.2f, b: %.2f\n", x, y, z, r, g, b);
	}
};