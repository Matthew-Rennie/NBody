#pragma once

#include "Object3d.h"
#include <vector>

class GravitySolverBase
{
public:

	virtual bool CalculateForces(std::vector<Object3d>& objects) = 0;
	ttvec3::BigFloat GConstant() const { return m_gConstant; }

private:
	ttvec3::BigFloat m_gConstant = 10.0;
};