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
			ttvec3 pos1 = objects[i].Position();
			ttvec3 pos2 = objects[j].Position();

			ttvec3 diff = pos2 - pos1;
			ttvec3::BigFloat r = diff.Length();

			const ttvec3::BigFloat cutoff(0.1);

			if (r < cutoff)
				continue;

			ttvec3::BigFloat m1 = objects[i].Mass();
			ttvec3::BigFloat m2 = objects[j].Mass();
			ttvec3::BigFloat F = (GConstant() * m1 * m2) / (r * r);

			ttvec3 dir = diff / r;

			objects[i].ApplyForce(dir, F);
			objects[j].ApplyForce(-dir, F);
		}
	}

	return true;
}

ttvec3::BigFloat GravitySolverBruteForce::CalcForce(const Object3d& first, const Object3d& second) const
{
	ttvec3 diff = first.Position() - second.Position();

	ttvec3::BigFloat r = diff.Length();

	if (r < 0.25)
		return 0.0;

	ttvec3::BigFloat m1 = first.Mass();
	ttvec3::BigFloat m2 = second.Mass();

	ttvec3::BigFloat F = (GConstant() * m1 * m2) / (r * r);

	return F;
}

ttvec3 GravitySolverBruteForce::CalcDirection(const Object3d& first, const Object3d& second) const
{
	ttvec3 diff = second.Position() - first.Position();

	ttvec3::BigFloat r = diff.Length();
	if (r == 0)
		return ttvec3();

	diff = diff / r;
	return diff;
}