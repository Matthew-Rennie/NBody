#pragma once
#include "glm/vec3.hpp"

struct Force
{
	Force(const glm::dvec3& d, const double& m)
	{
		dir = d;
		mag = m;
	}

	glm::dvec3 dir;
	double mag = 0.f;
};