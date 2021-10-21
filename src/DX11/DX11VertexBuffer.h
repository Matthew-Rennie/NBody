#pragma once

#include <d3d11.h>
#include <directxmath.h>

namespace dx11
{
	class DX11Renderer;

	using IndexType = unsigned long;

	struct VertexType
	{
		float position[3] = {};
		float texture[2] = {};
		float normal[3] = {};
	};

	class DX11VertexBuffer
	{
	public:

		struct DX11VertexType
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 tex;
			DirectX::XMFLOAT3 nor;
		};

		~DX11VertexBuffer();

		void init(dx11::DX11Renderer* renderer, VertexType* vertex_buffer, size_t vertex_count, IndexType* index_buffer, size_t index_count);

		size_t VertexCount() const { return m_vertex_count; }
		size_t IndexCount() const { return m_index_count; }

		ID3D11Buffer* VertexBuffer() { return m_vertexBuffer; }
		ID3D11Buffer** pVertexBuffer() { return &m_vertexBuffer; }
		ID3D11Buffer* IndexBuffer() { return m_indexBuffer; }
		ID3D11Buffer** pIndexBuffer() { return &m_indexBuffer; }

	protected:

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;

		HRESULT CreateVertexBuffer(ID3D11Device* device, DX11VertexType* vertex_buffer, size_t vertex_count);
		HRESULT CreateIndexBuffer(ID3D11Device* device, IndexType* index_buffer, size_t index_count);

		size_t m_vertex_count = 0;
		size_t m_index_count = 0;
	};
}
