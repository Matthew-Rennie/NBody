#include "stdafx.hpp"
#include "Object3d.h"

#include "DX11/DX11Renderer.h"
#include "DX11/DX11VertexBuffer.h"
#include "DX11/DX11Shader.h"
#include "graphics/TextureManager.h"
#include "glm/glm.hpp"
#include "graphics/CustomCamera.h"
#include "glm/fwd.hpp"
void Object3d::Update(const double dt)
{
	glm::dvec3 acceleration = glm::dvec3(0.f);

	for (auto& force : m_forces)
	{
		acceleration += force.dir * force.mag;
	}
	m_forces.clear();

	acceleration /= Mass();
	acceleration *= dt;

	m_position += m_velocity * dt;
	m_velocity += acceleration * dt;
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
	w = glm::scale(w, glm::dvec3(0.1, 0.1, 0.1));

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