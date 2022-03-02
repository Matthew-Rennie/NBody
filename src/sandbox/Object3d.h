#pragma once

#include "glm/glm.hpp"
#include "DX11/DX11VertexBuffer.h"

namespace dx11
{
	class DX11Renderer;
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

	void ApplyForce(const glm::dvec3 force) { m_force += force; }
	inline void ApplyForce(const glm::dvec3 dir, const double mag) { ApplyForce(dir * mag); }

	void SetVertexBuff(dx11::DX11VertexBuffer<dx11::VertexType>* vbuff) { m_vbuffer = vbuff; }

	double KineticEnergy() const;
	double PotentialEnergy(const Object3d& other, double Gconstant) const;

	double Radius();

	glm::dvec3 LastTickForce() { return m_lastTickForce; }

	void SetTexture(const std::string& name) { m_textureName = name; }

protected:

	std::string m_textureName = "default_texture";

	glm::dvec3 m_lastTickForce = glm::dvec3(0.f);
	glm::dvec3 m_force;

	double m_mass = 0.f;
	glm::dvec3 m_position = glm::dvec3(0.f);
	glm::dvec3 m_velocity = glm::dvec3(0.f);

	graphics::TextureManager* m_textureManager = nullptr;
	dx11::DX11VertexBuffer<dx11::VertexType>* m_vbuffer = nullptr;
};