#pragma once

#include <vector>
#include <array>
#include "glm/vec3.hpp"

class Object3d;

namespace core::TTM
{
	class ThreadTaskManager;
}

class OctTreeNodeMulti
{
public:

	~OctTreeNodeMulti();

	OctTreeNodeMulti* m_parent = nullptr;
	std::vector<Object3d*> m_objects;
	std::array<OctTreeNodeMulti*, 8> m_children = {};

	bool SubDivide();
	static void CalculateMass(OctTreeNodeMulti* _this, core::TTM::ThreadTaskManager* ttm);
	static void CalculateForces(OctTreeNodeMulti* _this, core::TTM::ThreadTaskManager* ttm);

	glm::dvec3 m_centrePoint = {};
	double m_lenPointToEdge = 0.f;

	glm::dvec3 m_centreOfMass = {};
	double m_mass = 0;

	void Reset();

private:

	void CalcForceRecursive(const OctTreeNodeMulti* node, const OctTreeNodeMulti* lastNode) const;
};