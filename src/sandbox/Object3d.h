#pragma once

#include "Force.h"
#include "ttvec3.h"

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

	void Update(const ttvec3::BigFloat& dt);
	void Render(dx11::DX11Renderer* renderer, graphics::CustomCamera* camera);

	ttvec3::BigFloat Mass() const { return m_mass; }
	ttvec3 Position() const { return m_position; }
	ttvec3 Velocity() const { return m_velocity; }

	void SetMass(const double mass) { m_mass = mass; }
	void SetPosition(const ttvec3 pos) { m_position = pos; }
	void SetVelocity(const ttvec3 vel) { m_velocity = vel; }

	void ApplyForce(const Force force) { m_forces.push_back(force); }
	void ApplyForce(const ttvec3 dir, const ttvec3::BigFloat mag) { m_forces.emplace_back(dir, mag); }

	void SetVertexBuff(dx11::DX11VertexBuffer* vbuff) { m_vbuffer = vbuff; }

	ttvec3::BigFloat KineticEnergy() const;
	ttvec3::BigFloat PotentialEnergy(const Object3d& other, ttvec3::BigFloat Gconstant) const;

protected:

	std::vector<Force> m_forces;

	ttvec3::BigFloat m_mass = 0.0;
	ttvec3 m_position;
	ttvec3 m_velocity;

	graphics::TextureManager* m_textureManager = nullptr;
	dx11::DX11VertexBuffer* m_vbuffer = nullptr;
};