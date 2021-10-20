#pragma once
#include "d3d11.h"

namespace dx11
{
	// the data here is copied into constant buffers
	struct ShaderDataEntry
	{
		static const size_t buffer_size = 256;
		char buffer[buffer_size] = {};
		bool enable = false;
	};

	// collection of all the constant buffers with a shader
	struct ShaderData
	{
		static const size_t num_slots = 8;
		ShaderDataEntry vs_data[num_slots] = {};
		ShaderDataEntry ps_data[num_slots] = {};
		ShaderDataEntry hs_data[num_slots] = {};
		ShaderDataEntry ds_data[num_slots] = {};
		ShaderDataEntry gs_data[num_slots] = {};
	};

	class DX11_Renderer;

	class DX11Shader
	{
	public:

		DX11Shader(dx11::DX11_Renderer* renderer);
		~DX11Shader();
		void Release();

		bool load_PS(wchar_t* filename);
		bool load_VS(wchar_t* filename);

		void run(ID3D11DeviceContext* context, UINT index_count);

	protected:

		ID3D11Device* m_device = nullptr;

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11HullShader* m_hullShader = nullptr;
		ID3D11DomainShader* m_domainShader = nullptr;
		ID3D11GeometryShader* m_geometryShader = nullptr;

		ID3D11InputLayout* m_layout = nullptr;
	};
}
