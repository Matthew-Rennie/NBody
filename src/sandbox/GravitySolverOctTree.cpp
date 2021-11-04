#include "stdafx.hpp"
#include "GravitySolverOctTree.h"
#include "OctTreeNode.h"

bool GravitySolverOctTree::CalculateForces(std::vector<Object3d>& objects)
{
	OctTreeNode* root = BuildTree(objects);
	root->CalculateMass();
	root->CalculateForces();
	SAFE_DELETE(root);
	return true;
}

OctTreeNode* GravitySolverOctTree::BuildTree(std::vector<Object3d>& objects)
{
	if (objects.size() < 2)
		return nullptr;

	OctTreeNode* root = new OctTreeNode();

	root->m_centrePoint = { 0.0,0.0,0.0 };

	for (auto& c : objects)
		root->m_objects.push_back(&c);

	double longestAxis = 0.0;

	auto longestAxisLambda = [&](double value)
	{
		double v = fabs(value);
		if (v > longestAxis)
			longestAxis = v;
	};

	for (const auto& o : objects)
	{
		longestAxisLambda(o.Position().x);
		longestAxisLambda(o.Position().y);
		longestAxisLambda(o.Position().z);
	}

	longestAxis = floor(longestAxis + 1.0);

	root->m_lenPointToEdge = longestAxis;

	root->SubDivide();
	return root;
}