#pragma once

#include "GravitySolverBase.h"

class OctTreeNode;

class GravitySolverOctTree : public GravitySolverBase
{
public:

	bool CalculateForces(std::vector<Object3d>& objects) override;

private:

	OctTreeNode* BuildTree(std::vector<Object3d>& objects);
};