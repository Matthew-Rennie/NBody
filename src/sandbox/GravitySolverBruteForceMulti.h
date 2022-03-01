#pragma once

#include "GravitySolverBase.h"

namespace core::TTM
{
	class ThreadTaskManager;
}

class GravitySolverBruteForceMulti : public GravitySolverBase
{
public:

	GravitySolverBruteForceMulti();
	~GravitySolverBruteForceMulti();

	bool CalculateForces(std::vector<Object3d>& objects) override;

private:

	static double CalcForce(const Object3d& first, const Object3d& second);
	static glm::dvec3 CalcDirection(const Object3d& first, const Object3d& second);

	// func that is run on the other thread
	static void ComputeObject(std::vector<Object3d>* objects, size_t index);

	core::TTM::ThreadTaskManager* m_ttm = nullptr;

};