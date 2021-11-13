#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <stdexcept>
#include "DX11Vertex.h"

namespace dx11
{
	class DX11Renderer;

	using IndexType = unsigned long;

	//struct VertexType
	//{
	//	float position[3] = {};
	//	float texture[2] = {};
	//	float normal[3] = {};
	//};

	template<typename _VertexType_>
	class DX11VertexBuffer
	{
		//static_assert(sizeof(_VertexType_) % 16 == 0);
	public:
		//struct DX11VertexType
		//{
		//	DirectX::XMFLOAT3 pos;
		//	DirectX::XMFLOAT2 tex;
		//	DirectX::XMFLOAT3 nor;
		//};

		~DX11VertexBuffer();

		void init(dx11::DX11Renderer* renderer, _VertexType_* vertex_buffer, size_t vertex_count, IndexType* index_buffer, size_t index_count);

		size_t& VertexCount() { return m_vertex_count; }
		size_t& IndexCount() { return m_index_count; }

		ID3D11Buffer* VertexBuffer() { return m_vertexBuffer; }
		ID3D11Buffer** pVertexBuffer() { return &m_vertexBuffer; }
		ID3D11Buffer* IndexBuffer() { return m_indexBuffer; }
		ID3D11Buffer** pIndexBuffer() { return &m_indexBuffer; }

	protected:

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;

		HRESULT CreateVertexBuffer(ID3D11Device* device, const _VertexType_* vertex_buffer, size_t vertex_count);
		HRESULT CreateIndexBuffer(ID3D11Device* device, IndexType* index_buffer, size_t index_count);

		size_t m_vertex_count = 0;
		size_t m_index_count = 0;
	};

	template<typename _VertexType_>
	HRESULT dx11::DX11VertexBuffer<_VertexType_>::CreateIndexBuffer(ID3D11Device* device, IndexType* index_buffer, size_t index_count)
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

	template<typename _VertexType_>
	dx11::DX11VertexBuffer<_VertexType_>::~DX11VertexBuffer()
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

	template<typename _VertexType_>
	void dx11::DX11VertexBuffer<_VertexType_>::init(dx11::DX11Renderer* renderer, _VertexType_* vertex_buffer, size_t vertex_count, IndexType* index_buffer, size_t index_count)
	{
		D3D* d3d = renderer->get_d3d();
		if (!d3d)
		{
			throw std::runtime_error("D3D was not initialised");
		}

		//_VertexType_* vb = new _VertexType_[vertex_count];
		//memcpy(vb, vertex_buffer, vertex_count * sizeof(_VertexType_));

		CreateVertexBuffer(d3d->getDevice(), vertex_buffer, vertex_count);
		CreateIndexBuffer(d3d->getDevice(), index_buffer, index_count);

		m_vertex_count = vertex_count;
		m_index_count = index_count;

		//delete[] vb;
	}

	template<typename _VertexType_>
	HRESULT dx11::DX11VertexBuffer<_VertexType_>::CreateVertexBuffer(ID3D11Device* device, const _VertexType_* vertex_buffer, size_t vertex_count)
	{
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA sr_data;

		// Set up the description of the static vertex buffer.
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(_VertexType_) * (UINT)vertex_count;
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
}
