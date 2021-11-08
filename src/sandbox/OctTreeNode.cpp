#include "stdafx.hpp"
#include "OctTreeNode.h"
#include "Object3d.h"

OctTreeNode::~OctTreeNode()
{
	SAFE_DELETE_STL_CONTAINER(m_children);
}

bool OctTreeNode::SubDivide()
{
	assert(m_objects.size() > 1);

	double halfLen = this->m_lenPointToEdge / 2.0;

	for (auto& c : m_children)
	{
		c = new OctTreeNode();
		c->m_parent = this;
		c->m_lenPointToEdge = halfLen;
	}

	glm::dvec3 childCentrePoint0 = { m_centrePoint + glm::dvec3(+halfLen, +halfLen, +halfLen) }; // +++
	glm::dvec3 childCentrePoint1 = { m_centrePoint + glm::dvec3(+halfLen, +halfLen, -halfLen) }; // ++-
	glm::dvec3 childCentrePoint2 = { m_centrePoint + glm::dvec3(+halfLen, -halfLen, +halfLen) }; // +-+
	glm::dvec3 childCentrePoint3 = { m_centrePoint + glm::dvec3(+halfLen, -halfLen, -halfLen) }; // +--
	glm::dvec3 childCentrePoint4 = { m_centrePoint + glm::dvec3(-halfLen, +halfLen, +halfLen) }; // -++
	glm::dvec3 childCentrePoint5 = { m_centrePoint + glm::dvec3(-halfLen, +halfLen, -halfLen) }; // -+-
	glm::dvec3 childCentrePoint6 = { m_centrePoint + glm::dvec3(-halfLen, -halfLen, +halfLen) }; // --+
	glm::dvec3 childCentrePoint7 = { m_centrePoint + glm::dvec3(-halfLen, -halfLen, -halfLen) }; // ---

	m_children[0]->m_centrePoint = childCentrePoint0;
	m_children[1]->m_centrePoint = childCentrePoint1;
	m_children[2]->m_centrePoint = childCentrePoint2;
	m_children[3]->m_centrePoint = childCentrePoint3;
	m_children[4]->m_centrePoint = childCentrePoint4;
	m_children[5]->m_centrePoint = childCentrePoint5;
	m_children[6]->m_centrePoint = childCentrePoint6;
	m_children[7]->m_centrePoint = childCentrePoint7;

	for (auto& o : m_objects)
	{
		const glm::dvec3& pos = o->Position();

		bool greater_x = pos.x > m_centrePoint.x;
		bool greater_y = pos.y > m_centrePoint.y;
		bool greater_z = pos.z > m_centrePoint.z;

		if (greater_x && greater_y && greater_z) // +++
		{
			m_children[0]->m_objects.push_back(o);
			continue;
		}
		if (greater_x && greater_y && !greater_z) // ++-
		{
			m_children[1]->m_objects.push_back(o);
			continue;
		}
		if (greater_x && !greater_y && greater_z) // +-+
		{
			m_children[2]->m_objects.push_back(o);
			continue;
		}
		if (greater_x && !greater_y && !greater_z) // +--
		{
			m_children[3]->m_objects.push_back(o);
			continue;
		}
		if (!greater_x && greater_y && greater_z) // -++
		{
			m_children[4]->m_objects.push_back(o);
			continue;
		}
		if (!greater_x && greater_y && !greater_z) // -+-
		{
			m_children[5]->m_objects.push_back(o);
			continue;
		}
		if (!greater_x && !greater_y && greater_z) // --+
		{
			m_children[6]->m_objects.push_back(o);
			continue;
		}
		if (!greater_x && !greater_y && !greater_z) // ---
		{
			m_children[7]->m_objects.push_back(o);
			continue;
		}

		//fallback
		m_children[0]->m_objects.push_back(o);
	}

	m_objects.clear();

	for (auto& c : m_children)
	{
		if (c->m_objects.size() > 1)
			c->SubDivide();
	}

	return true;
}

bool OctTreeNode::CalculateMass()
{
	m_mass = 0;
	m_centreOfMass = {};

	if (m_objects.size() == 1)
	{
		m_mass = m_objects[0]->Mass();
		m_centreOfMass = m_objects[0]->Position();
		return true;
	}

	// dead end, no more objects here
	if (m_children[0] == nullptr)
		return true;

	for (auto& c : m_children)
		c->CalculateMass();

	for (auto& c : m_children)
	{
		if (c->m_mass == 0)
			continue;

		if (this->m_mass == 0)
		{
			this->m_mass = c->m_mass;
			this->m_centreOfMass = c->m_centreOfMass;
			continue;
		}

		glm::dvec3 diff = c->m_centreOfMass - this->m_centreOfMass;

		double ratio = c->m_mass / (c->m_mass + this->m_mass);

		diff *= ratio;

		m_centreOfMass += diff;
		m_mass += c->m_mass;
	}
}

bool OctTreeNode::CalculateForces()
{
	if (m_objects.size() == 1)
	{
		// traverse up tree

		glm::dvec3 objectPos = m_objects[0]->Position();
		double objectMass = m_objects[0]->Mass();

		OctTreeNode* lastNode = this;
		OctTreeNode* node = this->m_parent;
		while (node)
		{
			for (const auto& c : node->m_children)
			{
				if (c->m_mass == 0.0)
					continue;

				if (c == lastNode)
					continue;

				glm::dvec3 dir = c->m_centreOfMass - objectPos;
				double r = glm::length(dir);
				if (r < 0.25)
					continue;
				dir = dir / r;

				double m1 = c->m_mass;
				double m2 = objectMass;
				double G = 1; // #TODO replace

				double F = (G * m1 * m2) / (r * r);

				m_objects[0]->ApplyForce(dir, F);
			}

			lastNode = node;
			node = node->m_parent;
		}
	}

	// dead end, no more objects here
	if (m_children[0] == nullptr)
		return true;

	for (auto& c : m_children)
		c->CalculateForces();

	return true;
}