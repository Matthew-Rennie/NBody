#pragma once
#include "glm/vec3.hpp"

struct Force
{
	Force(const glm::vec3& d, const float& m)
	{
		dir = d;
		mag = m;
	}

	glm::vec3 dir;
	float mag = 0.f;
};