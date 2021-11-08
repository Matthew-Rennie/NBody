#pragma once

#include "glm/glm.hpp"
#include "Force.h"

namespace dx11
{
	class DX11Renderer;
	class DX11VertexBuffer;
	struct DX11ShaderData;
}

namespace graphics
{
	class TextureManager;
	class CustomCamera;
}

class Object3d
{
public:

	Object3d(graphics::TextureManager* texManager)
	{
		m_textureManager = texManager;
	}

	void Update(const double dt);
	void Render(dx11::DX11Renderer* renderer, graphics::CustomCamera* camera);

	double Mass() const { return m_mass; }
	glm::dvec3 Position() const { return m_position; }
	glm::dvec3 Velocity() const { return m_velocity; }

	void SetMass(const double mass) { m_mass = mass; }
	void SetPosition(const glm::dvec3 pos) { m_position = pos; }
	void SetVelocity(const glm::dvec3 vel) { m_velocity = vel; }

	void ApplyForce(const Force force) { m_forces.push_back(force); }
	void ApplyForce(const glm::dvec3 dir, const double mag) { m_forces.emplace_back(dir, mag); }

	void SetVertexBuff(dx11::DX11VertexBuffer* vbuff) { m_vbuffer = vbuff; }

	double KineticEnergy() const;
	double PotentialEnergy(const Object3d& other, double Gconstant) const;

	double Radius();

	glm::dvec3 LastTickForce() { return m_lastTickForce; }

protected:

	glm::dvec3 m_lastTickForce;

	std::vector<Force> m_forces;

	double m_mass = 0.f;
	glm::dvec3 m_position = glm::dvec3(0.f);
	glm::dvec3 m_velocity = glm::dvec3(0.f);

	graphics::TextureManager* m_textureManager = nullptr;
	dx11::DX11VertexBuffer* m_vbuffer = nullptr;
};