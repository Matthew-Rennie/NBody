#include "stdafx.hpp"
#include "GravitySolverBruteForceMulti.h"
#include "core/ThreadTaskManager.h"

GravitySolverBruteForceMulti::GravitySolverBruteForceMulti()
{
	m_ttm = new core::TTM::ThreadTaskManager(std::thread::hardware_concurrency());
}

GravitySolverBruteForceMulti::~GravitySolverBruteForceMulti()
{
	SAFE_DELETE(m_ttm);
}

bool GravitySolverBruteForceMulti::CalculateForces(std::vector<Object3d>& objects)
{
	for (int i = 0; i < objects.size(); i++)
	{
		m_ttm->add_task(ComputeObject, &objects, i);
	}
	m_ttm->wait();

	return true;
}

double GravitySolverBruteForceMulti::CalcForce(const Object3d& first, const Object3d& second)
{
	glm::dvec3 diff = first.Position() - second.Position();

	double r = glm::length(diff);
	double m1 = first.Mass();
	double m2 = second.Mass();
	double G = 1; // #TODO replace

	double F = (G * m1 * m2) / (r * r);

	if (r < 0.25)
		return 0.0;

	return F;
}

glm::dvec3 GravitySolverBruteForceMulti::CalcDirection(const Object3d& first, const Object3d& second)
{
	glm::dvec3 diff = second.Position() - first.Position();
	double r = glm::length(diff);
	diff = diff / r;
	return diff;
}

void GravitySolverBruteForceMulti::ComputeObject(std::vector<Object3d>* objects, size_t index)
{
	for (int i = 0; i < objects->size(); i++)
	{
		if(i == index) continue;

		double f = CalcForce((*objects)[i], (*objects)[index]);
		glm::dvec3 dir = CalcDirection((*objects)[i], (*objects)[index]);

		(*objects)[index].ApplyForce(-dir, f);
	}
}
