#pragma once
#include "glm/vec3.hpp"
#include "ttvec3.h"

struct Force
{
	Force(const ttvec3& d, const ttvec3::BigFloat& m)
	{
		dir = d;
		mag = m;
	}

	ttvec3 dir;
	ttvec3::BigFloat mag = 0.0;
};