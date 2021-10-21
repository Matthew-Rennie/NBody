#include "stdafx.hpp"
#include "DX11VertexBuffer.h"
#include "DX11Renderer.h"
#include <stdexcept>
#include "DX11Renderer.h"

dx11::DX11VertexBuffer::~DX11VertexBuffer()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void dx11::DX11VertexBuffer::init(dx11::DX11Renderer* renderer, VertexType* vertex_buffer, size_t vertex_count, IndexType* index_bufer, size_t index_count)
{
	DX11Renderer* dx11_renderer = dynamic_cast<DX11Renderer*>(renderer);
	if (!dx11_renderer)
	{
		throw std::runtime_error("could not conver core::BaseRenderer to dx11::DX11Renderer");
	}

	D3D* d3d = dx11_renderer->get_d3d();
	if (!d3d)
	{
		throw std::runtime_error("D3D was not initialised");
	}

	DX11VertexType* vb = new DX11VertexType[vertex_count];

	for (int i = 0; i < vertex_count; i++)
	{
		vb[i].pos = { vertex_buffer[i].position[0],vertex_buffer[i].position[1] ,vertex_buffer[i].position[2] };
		vb[i].tex = { vertex_buffer[i].texture[0], vertex_buffer[i].texture[1] };
		vb[i].nor = { vertex_buffer[i].normal[0],vertex_buffer[i].normal[1] ,vertex_buffer[i].normal[2] };
	}

	CreateVertexBuffer(d3d->getDevice(), vb, vertex_count);
	CreateIndexBuffer(d3d->getDevice(), index_bufer, index_count);

	m_vertex_count = vertex_count;
	m_index_count = index_count;

	delete[] vb;
}

HRESULT dx11::DX11VertexBuffer::CreateVertexBuffer(ID3D11Device* device, DX11VertexType* vertex_buffer, size_t vertex_count)
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA sr_data;

	// Set up the description of the static vertex buffer.
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(DX11VertexType) * (UINT)vertex_count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	// Give the sub-resource structure a pointer to the vertex data.
	sr_data.pSysMem = vertex_buffer;
	sr_data.SysMemPitch = 0;
	sr_data.SysMemSlicePitch = 0;

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	HRESULT result = device->CreateBuffer(&desc, &sr_data, &m_vertexBuffer);
	return result;
}

HRESULT dx11::DX11VertexBuffer::CreateIndexBuffer(ID3D11Device* device, IndexType* index_buffer, size_t index_count)
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA sr_data;

	// Set up the description of the static index buffer.
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(IndexType) * (UINT)index_count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	// Give the sub-resource structure a pointer to the index data.
	sr_data.pSysMem = index_buffer;
	sr_data.SysMemPitch = 0;
	sr_data.SysMemSlicePitch = 0;

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	HRESULT result = device->CreateBuffer(&desc, &sr_data, &m_indexBuffer);
	return result;
}