#pragma once

#include "Object3d.h"
#include <vector>

class GravitySolverBase
{
public:
	virtual bool CalculateForces(std::vector<Object3d>& objects) = 0;
};