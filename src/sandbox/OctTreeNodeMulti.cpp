#include "stdafx.hpp"
#include "OctTreeNodeMulti.h"
#include "Object3d.h"
#include "core/ThreadTaskManager.h"

#define BARNES_HUTT_MULTI_THETA DOUBLE(1.2)
#define BARNES_HUTT_MULTI_MIN_DISTANCE DOUBLE(0.25)

OctTreeNodeMulti::~OctTreeNodeMulti()
{
	SAFE_DELETE_STL_CONTAINER(m_children);
}

bool OctTreeNodeMulti::SubDivide()
{
	assert(m_objects.size() > 1);

	double halfLen = this->m_lenPointToEdge / 2.0;

	for (auto& c : m_children)
	{
		c = new OctTreeNodeMulti();
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

void OctTreeNodeMulti::CalculateMass(OctTreeNodeMulti* _this, core::TTM::ThreadTaskManager* ttm)
{
	_this->m_mass = 0;
	_this->m_centreOfMass = {};

	if (_this->m_objects.size() == 1)
	{
		_this->m_mass = _this->m_objects[0]->Mass();
		_this->m_centreOfMass = _this->m_objects[0]->Position();
		return;
	}

	// dead end, no more objects here
	if (_this->m_children[0] == nullptr)
		return;

	for (auto& c : _this->m_children)
	{
		OctTreeNodeMulti::CalculateMass(c, ttm);
	}

	for (auto& c : _this->m_children)
	{
		if (c->m_mass == 0)
			continue;

		if (_this->m_mass == 0)
		{
			_this->m_mass = c->m_mass;
			_this->m_centreOfMass = c->m_centreOfMass;
			continue;
		}

		glm::dvec3 diff = c->m_centreOfMass - _this->m_centreOfMass;

		double ratio = c->m_mass / (c->m_mass + _this->m_mass);

		diff *= ratio;

		_this->m_centreOfMass += diff;
		_this->m_mass += c->m_mass;
	}
}

void OctTreeNodeMulti::CalculateForces(OctTreeNodeMulti* _this, core::TTM::ThreadTaskManager* ttm)
{
	if (_this->m_objects.size() == 1)
	{
		// traverse up tree

		glm::dvec3 objectPos = _this->m_objects[0]->Position();
		double objectMass = _this->m_objects[0]->Mass();

		OctTreeNodeMulti* lastNode = _this;
		OctTreeNodeMulti* node = _this->m_parent;
		while (node)
		{
			_this->CalcForceRecursive(node, lastNode);

			lastNode = node;
			node = node->m_parent;
		}
	}

	// dead end, no more objects here
	if (_this->m_children[0] == nullptr)
		return;

	for (auto& c : _this->m_children)
	{
		ttm->add_task(OctTreeNodeMulti::CalculateForces, c, ttm);
	}

}

void OctTreeNodeMulti::Reset()
{
	m_objects.clear();
	memset(m_children.data(), 0, m_children.size() * sizeof(Object3d*));
}

void OctTreeNodeMulti::CalcForceRecursive(const OctTreeNodeMulti* node, const OctTreeNodeMulti* lastNode) const
{
	glm::dvec3 objectPos = m_objects[0]->Position();
	double objectMass = m_objects[0]->Mass();

	for (const auto& c : node->m_children)
	{
		if (c == nullptr) continue;

		if (c->m_mass == 0.0) continue;

		if (c == lastNode) continue;

		glm::dvec3 dir = c->m_centreOfMass - objectPos;
		double r = glm::length(dir);
		double d = (c->m_lenPointToEdge * 2) / r;

		if (d > BARNES_HUTT_MULTI_THETA)
		{
			// use COM of node
			if (r < BARNES_HUTT_MULTI_MIN_DISTANCE)
				continue;

			dir = dir / r;

			double m1 = c->m_mass;
			double m2 = objectMass;
			double G = 1; // #TODO replace

			double F = (G * m1 * m2) / (r * r);

			m_objects[0]->ApplyForce(dir, F);
		}
		else if (c->m_objects.size() == 1)
		{
			// only one object, use node COM
			if (r < BARNES_HUTT_MULTI_MIN_DISTANCE)
				continue;
			dir = dir / r;

			double m1 = c->m_mass;
			double m2 = objectMass;
			double G = 1; // #TODO replace

			double F = (G * m1 * m2) / (r * r);

			m_objects[0]->ApplyForce(dir, F);
		}
		else
		{
			CalcForceRecursive(c, nullptr);
			// node is too close, use each sub node
		}
	}
}
