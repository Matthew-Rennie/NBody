#pragma once

#include "GravitySolverBase.h"

class GravitySolverBruteForce : public GravitySolverBase
{
public:

	bool CalculateForces(std::vector<Object3d>& objects) override;

private:

	double CalcForce(const Object3d& first, const Object3d& second) const;
	glm::dvec3 CalcDirection(const Object3d& first, const Object3d& second) const;
};