#include "stdafx.hpp"
#include "DX11LineRenderer.h"

#include "DX11Renderer.h"
#include "DX11VertexBuffer.h"
#include "DX11Shader.h"
#include "DX11Vertex.h"

dx11::DX11LineRenderer::DX11LineRenderer(DX11Renderer* renderer) :
	m_renderer(renderer)
{
	m_shader = new DX11Shader(m_renderer);
	m_shader->load_VS(L"line_shader_vs.cso", dx11::VertexLayout::LineVertexTypeLayout, sizeof(dx11::VertexLayout::LineVertexTypeLayout));
	m_shader->load_PS(L"line_shader_ps.cso");
}

dx11::DX11LineRenderer::~DX11LineRenderer()
{
	delete m_shader;
	m_shader = nullptr;
}

void dx11::DX11LineRenderer::AddLine(glm::dvec3 p1, glm::dvec3 p2, dx11::Color color)
{
	m_lines.emplace_back(p1, p2, color);
}

void dx11::DX11LineRenderer::AddLine(Line line)
{
	m_lines.push_back(line);
}

void dx11::DX11LineRenderer::Render(glm::mat4x4 view, bool clearLines)
{
	size_t vertexCount = m_lines.size() * 2;
	size_t indexCount = m_lines.size() * 2;

	LineVertexType* vtypebuff = new LineVertexType[vertexCount];

	for (int i = 0; i < m_lines.size(); i++)
	{
		const Line& line = m_lines[i];

		int index = i * 2;
		vtypebuff[index].pos = DirectX::XMFLOAT3((float)line.p1.x, (float)line.p1.y, (float)line.p1.z);
		vtypebuff[index + 1].pos = DirectX::XMFLOAT3((float)line.p2.x, (float)line.p2.y, (float)line.p2.z);

		vtypebuff[index].col = DirectX::XMFLOAT3(line.color.r, line.color.g, line.color.b);
		vtypebuff[index + 1].col = DirectX::XMFLOAT3(line.color.r, line.color.g, line.color.b);
	}

	IndexType* indexbuff = new IndexType[indexCount];
	for (int i = 0; i < indexCount; i++)
	{
		indexbuff[i] = i;
	}

	DX11VertexBuffer<LineVertexType>* vBuffer = new DX11VertexBuffer<LineVertexType>();
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