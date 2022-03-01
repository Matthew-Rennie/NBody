#include "stdafx.hpp"
#include "Object3d.h"

#include "DX11/DX11Renderer.h"
#include "DX11/DX11VertexBuffer.h"
#include "DX11/DX11Shader.h"
#include "graphics/TextureManager.h"
#include "glm/glm.hpp"
#include "graphics/CustomCamera.h"

void Object3d::Update(const double dt)
{
	m_lastTickForce = m_force;

	m_force /= m_mass;

	m_velocity += m_force * dt;
	m_position += m_velocity * dt;

	m_force = glm::dvec3();
}

void Object3d::Render(dx11::DX11Renderer* renderer, graphics::CustomCamera* camera)
{
	struct BuffType
	{
		glm::mat4x4 world;
		glm::mat4x4 view;
		glm::mat4x4 projection;
	};
	static_assert(sizeof(BuffType) <= 512);

	dx11::DX11ShaderData shaderData;
	shaderData.vs_data[0].enable = true;

	BuffType* bufftype = (BuffType*)shaderData.vs_data[0].buffer;

	glm::dmat4x4 w = glm::translate(glm::dmat4x4(1.f), m_position);

	double r = Radius();
	w = glm::scale(w, glm::dvec3(r, r, r));

	bufftype->world = (glm::mat4x4)glm::transpose(w);
	bufftype->view = glm::transpose(camera->getViewMatrix());
	bufftype->projection = glm::transpose(renderer->getProjectionMatrix(45));

	memcpy(shaderData.vs_data[0].buffer, bufftype, sizeof(BuffType));

	if (renderer && m_textureManager && m_vbuffer)
	{
		renderer->send_data(&shaderData);
		renderer->setTexture(m_textureManager->getTexture("bunny"), 0);
		renderer->render(m_vbuffer);
	}
}

double Object3d::KineticEnergy() const
{
	return 0.5f * Mass() * glm::length(Velocity()) * glm::length(Velocity());
}

double Object3d::PotentialEnergy(const Object3d& other, double Gconstant) const
{
	double r = glm::length(Position() - other.Position());

	return -(Mass() * other.Mass() * Gconstant) / r;
}

double Object3d::Radius()
{
	return std::cbrt(m_mass) * 0.1;
}