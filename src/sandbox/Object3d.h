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

	void Update(const float dt);
	void Render(dx11::DX11Renderer* renderer, graphics::CustomCamera* camera);

	float Mass() const { return m_mass; }
	glm::vec3 Position() const { return m_position; }
	glm::vec3 Velocity() const { return m_velocity; }

	void SetMass(const float mass) { m_mass = mass; }
	void SetPosition(const glm::vec3 pos) { m_position = pos; }
	void SetVelocity(const glm::vec3 vel) { m_velocity = vel; }

	void ApplyForce(const Force force) { m_forces.push_back(force); }
	void ApplyForce(const glm::vec3 dir, const float mag) { m_forces.emplace_back(dir, mag); }

	void SetVertexBuff(dx11::DX11VertexBuffer* vbuff) { m_vbuffer = vbuff; }

protected:

	std::vector<Force> m_forces;

	float m_mass = 0.f;
	glm::vec3 m_position = glm::vec3(0.f);
	glm::vec3 m_velocity = glm::vec3(0.f);

	graphics::TextureManager* m_textureManager = nullptr;
	dx11::DX11VertexBuffer* m_vbuffer = nullptr;
};