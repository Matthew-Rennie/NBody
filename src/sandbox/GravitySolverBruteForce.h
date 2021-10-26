#pragma once

#include "GravitySolverBase.h"
#include "Force.h"

class GravitySolverBruteForce : public GravitySolverBase
{
public:

	bool CalculateForces(std::vector<Object3d>& objects) override;

private:

	ttvec3::BigFloat CalcForce(const Object3d& first, const Object3d& second) const;
	ttvec3 CalcDirection(const Object3d& first, const Object3d& second) const;
};