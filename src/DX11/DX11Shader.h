#pragma once
#include "core/BaseShader.h"
#include "core/BaseRenderer.h"
#include "d3d11.h"

namespace dx11
{
	class DX11Shader : public core::BaseShader
	{
	public:
		DX11Shader(core::BaseRenderer* renderer);
		~DX11Shader();
		void Release() override;

		bool load_PS(wchar_t* filename) override;
		bool load_VS(wchar_t* filename) override;

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
