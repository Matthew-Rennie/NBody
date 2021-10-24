#include "stdafx.hpp"
#include "GravitySolverBruteForce.h"

bool GravitySolverBruteForce::CalculateForces(std::vector<Object3d>& objects)
{
	const size_t count = objects.size();

	std::vector<glm::vec3> forces;
	forces.resize(count);

	for (int i = 0; i < count; i++)
	{
		for (int j = i + 1; j < count; j++)
		{
			float f = CalcForce(objects[i], objects[j]);
			glm::vec3 dir = CalcDirection(objects[i], objects[j]);

			objects[i].ApplyForce(dir, f);
			objects[j].ApplyForce(-dir, f);
		}
	}

	return true;
}

float GravitySolverBruteForce::CalcForce(const Object3d& first, const Object3d& second) const
{
	glm::vec3 diff = first.Position() - second.Position();

	float r = glm::length(diff);
	float m1 = first.Mass();
	float m2 = second.Mass();
	float G = 1; // TODO replace

	float F = (G * m1 * m2) / (r * r);

	return F;
}

glm::vec3 GravitySolverBruteForce::CalcDirection(const Object3d& first, const Object3d& second) const
{
	glm::vec3 diff = second.Position() - first.Position();
	float r = glm::length(diff);

	diff = diff / r;
	return diff;
}