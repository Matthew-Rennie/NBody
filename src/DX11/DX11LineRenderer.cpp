#include "stdafx.hpp"
#include "DX11LineRenderer.h"

#include "DX11Renderer.h"
#include "DX11VertexBuffer.h"
#include "DX11Shader.h"

dx11::DX11LineRenderer::DX11LineRenderer(DX11Renderer* renderer) :
	m_renderer(renderer)
{
	m_shader = new DX11Shader(m_renderer);
	m_shader->load_VS(L"line_shader_vs.cso");
	m_shader->load_PS(L"line_shader_ps.cso");
}

dx11::DX11LineRenderer::~DX11LineRenderer()
{
	delete m_shader;
	m_shader = nullptr;
}

void dx11::DX11LineRenderer::Draw(glm::dvec3 p1, glm::dvec3 p2, glm::mat4x4 view) const
{
	VertexType vtypebuff[2] = {};

	vtypebuff[0].position[0] = p1.x;
	vtypebuff[0].position[1] = p1.y;
	vtypebuff[0].position[2] = p1.z;

	vtypebuff[1].position[0] = p2.x;
	vtypebuff[1].position[1] = p2.y;
	vtypebuff[1].position[2] = p2.z;

	IndexType indexbuff[2] = { 0,1 };

	DX11VertexBuffer* vBuffer = new DX11VertexBuffer();
	vBuffer->init(m_renderer, vtypebuff, 2, indexbuff, 2);

	struct BuffType
	{
		glm::mat4x4 world;
		glm::mat4x4 view;
		glm::mat4x4 projection;
	};
	static_assert(sizeof(BuffType) <= 512);

	BuffType* bufftype = new BuffType();

	bufftype->world = glm::transpose(glm::mat4x4(1));
	bufftype->view = glm::transpose(view);
	bufftype->projection = glm::transpose(m_renderer->getProjectionMatrix(45));

	dx11::DX11ShaderData shaderData;
	shaderData.vs_data[0].enable = true;
	memcpy(shaderData.vs_data[0].buffer, bufftype, sizeof(BuffType));

	m_renderer->set_shader(m_shader);
	m_renderer->send_data(&shaderData);
	m_renderer->render(vBuffer, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	delete bufftype;
	delete vBuffer;
}

void dx11::DX11LineRenderer::AddLine(glm::dvec3 p1, glm::dvec3 p2)
{
	m_lines.emplace_back(p1, p2);
}

void dx11::DX11LineRenderer::AddLine(Line line)
{
	m_lines.push_back(line);
}

void dx11::DX11LineRenderer::Render(glm::mat4x4 view, bool clearLines)
{
	size_t vertexCount = m_lines.size() * 2;
	size_t indexCount = m_lines.size() * 2;

	VertexType* vtypebuff = new VertexType[vertexCount];

	for (int i = 0; i < m_lines.size(); i++)
	{
		int index = i * 2;
		vtypebuff[index].position[0] = m_lines[i].p1.x;
		vtypebuff[index].position[1] = m_lines[i].p1.y;
		vtypebuff[index].position[2] = m_lines[i].p1.z;

		vtypebuff[index + 1].position[0] = m_lines[i].p2.x;
		vtypebuff[index + 1].position[1] = m_lines[i].p2.y;
		vtypebuff[index + 1].position[2] = m_lines[i].p2.z;
	}

	IndexType* indexbuff = new IndexType[indexCount];
	for (int i = 0; i < indexCount; i++)
	{
		indexbuff[i] = i;
	}

	DX11VertexBuffer* vBuffer = new DX11VertexBuffer();
	vBuffer->init(m_renderer, vtypebuff, vertexCount, indexbuff, indexCount);

	delete[] vtypebuff;
	vtypebuff = nullptr;

	delete[] indexbuff;
	indexbuff = nullptr;

	struct BuffType
	{
		glm::mat4x4 world;
		glm::mat4x4 view;
		glm::mat4x4 projection;
	};
	static_assert(sizeof(BuffType) <= 512);

	BuffType* bufftype = new BuffType();

	bufftype->world = glm::transpose(glm::mat4x4(1));
	bufftype->view = glm::transpose(view);
	bufftype->projection = glm::transpose(m_renderer->getProjectionMatrix(45));

	dx11::DX11ShaderData shaderData;
	shaderData.vs_data[0].enable = true;
	memcpy(shaderData.vs_data[0].buffer, bufftype, sizeof(BuffType));

	m_renderer->set_shader(m_shader);
	m_renderer->send_data(&shaderData);
	m_renderer->render(vBuffer, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	delete bufftype;
	delete vBuffer;

	if (clearLines)
		Clear();
}

void dx11::DX11LineRenderer::Clear()
{
	m_lines.clear();
}