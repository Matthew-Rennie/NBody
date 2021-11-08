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

	void DrawDebugGui();

private:

	unsigned long long count = 0;

	OctTreeNode* BuildTree(std::vector<Object3d>& objects);
	void RenderNode(const OctTreeNode* node, graphics::WireCubeRenderer* renderer) const;
	void DrawDebugGuiNode(OctTreeNode* node);

	// kept here so it can be rendered for debugging
	OctTreeNode* root = nullptr;
};