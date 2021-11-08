#pragma once

#include "GravitySolverBase.h"

namespace graphics
{
	class WireCubeRenderer;
}

class OctTreeNode;

class GravitySolverOctTree : public GravitySolverBase
{
public:
	~GravitySolverOctTree();

	bool CalculateForces(std::vector<Object3d>& objects) override;

	void RenderWireframe(graphics::WireCubeRenderer* renderer) const;

private:

	OctTreeNode* BuildTree(std::vector<Object3d>& objects);
	void RenderNode(const OctTreeNode* node, graphics::WireCubeRenderer* renderer) const;

	// kept here so it can be rendered for debugging
	OctTreeNode* root = nullptr;
};