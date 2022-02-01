#include "stdafx.hpp"
#include "GravitySolverOctTreeMulti.h"
#include "OctTreeNodeMulti.h"
#include "graphics/WireCubeRenderer.h"
#include "imGUI/imgui.h"

GravitySolverOctTreeMulti::GravitySolverOctTreeMulti()
{
	m_ttm = new core::TTM::ThreadTaskManager(std::thread::hardware_concurrency());
}

GravitySolverOctTreeMulti::~GravitySolverOctTreeMulti()
{
	SAFE_DELETE(m_ttm);
	SAFE_DELETE(root);
}

bool GravitySolverOctTreeMulti::CalculateForces(std::vector<Object3d>& objects)
{
	SAFE_DELETE(root);
	root = BuildTree(objects);

	m_ttm->add_task(OctTreeNodeMulti::CalculateMass, root, m_ttm);
	m_ttm->wait();
	m_ttm->add_task(OctTreeNodeMulti::CalculateForces, root, m_ttm);
	m_ttm->wait();

	return true;
}

OctTreeNodeMulti* GravitySolverOctTreeMulti::BuildTree(std::vector<Object3d>& objects)
{
	count++;

	if (objects.size() < 2)
		return nullptr;

	OctTreeNodeMulti* root = new OctTreeNodeMulti();

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

	// longestAxis = floor(longestAxis + 1.0);

	root->m_lenPointToEdge = longestAxis;

	root->SubDivide();
	return root;
}
