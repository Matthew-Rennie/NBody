#include "stdafx.hpp"
#include "GravitySolverBruteForce.h"

bool GravitySolverBruteForce::CalculateForces(std::vector<Object3d>& objects)
{
	const size_t count = objects.size();

	std::vector<glm::dvec3> forces;
	forces.resize(count);

	for (int i = 0; i < count; i++)
	{
		for (int j = i + 1; j < count; j++)
		{
			double f = CalcForce(objects[i], objects[j]);
			glm::dvec3 dir = CalcDirection(objects[i], objects[j]);

			objects[i].ApplyForce(dir, f);
			objects[j].ApplyForce(-dir, f);
		}
	}

	return true;
}

double GravitySolverBruteForce::CalcForce(const Object3d& first, const Object3d& second) const
{
	glm::dvec3 diff = first.Position() - second.Position();

	double r = glm::length(diff);
	double m1 = first.Mass();
	double m2 = second.Mass();
	double G = 1; // TODO replace

	double F = (G * m1 * m2) / (r * r);

	if (r < 0.25)
		return 0.0;

	return F;
}

glm::dvec3 GravitySolverBruteForce::CalcDirection(const Object3d& first, const Object3d& second) const
{
	glm::dvec3 diff = second.Position() - first.Position();
	double r = glm::length(diff);
	diff = diff / r;
	return diff;
}