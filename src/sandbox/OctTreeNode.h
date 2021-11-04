#pragma once

#include <vector>
#include <array>
#include "glm/vec3.hpp"

class Object3d;

class OctTreeNode
{
public:
	~OctTreeNode();

	OctTreeNode* m_parent = nullptr;
	std::vector<Object3d*> m_objects;
	std::array<OctTreeNode*, 8> m_children;

	bool SubDivide();
	bool CalculateMass();
	bool CalculateForces();

	glm::dvec3 m_centrePoint = {};
	double m_lenPointToEdge = 0.f;

	glm::dvec3 m_centreOfMass = {};
	double m_mass = 0;
};