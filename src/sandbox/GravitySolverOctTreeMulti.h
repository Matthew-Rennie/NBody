#pragma once

#include "GravitySolverBase.h"
#include "core/ThreadTaskManager.h"

namespace graphics
{
	class WireCubeRenderer;
}

class OctTreeNodeMulti;

class GravitySolverOctTreeMulti : public GravitySolverBase
{
public:
	GravitySolverOctTreeMulti();
	~GravitySolverOctTreeMulti();

	bool CalculateForces(std::vector<Object3d>& objects) override;

private:

	unsigned long long count = 0;

	core::TTM::ThreadTaskManager* m_ttm = nullptr; 
	OctTreeNodeMulti* BuildTree(std::vector<Object3d>& objects);

	OctTreeNodeMulti* root = nullptr;

};