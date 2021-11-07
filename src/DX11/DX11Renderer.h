#pragma once
#include <windows.h>
#include "core/Input.h"
#include "D3D.h"

#include <d3d11.h>
#include <directxmath.h>
#include "core/Window.h"
#include "ConstantBuffer.h"

#include "glm/glm.hpp"
#include "DX11Shader.h"

namespace graphics
{
	struct Texture;
};

namespace core
{
	class BaseVertexBuffer;
	class Window;
	class BaseShader;
	struct DX11ShaderData;
}

namespace dx11
{
	class DX11Shader;
	class DX11VertexBuffer;

	class DX11Renderer
	{
	public:

		DX11Renderer(core::Input* in);
		~DX11Renderer();

		bool init(core::Window* window);
		bool release();

		void begin_frame();
		void end_frame();
		void clear_display();

		void set_wireframe(bool b);
		bool get_wireframe();

		void set_vsync(bool b);
		bool get_vsync();

		void set_resolution(int width, int height);

		void render(dx11::DX11VertexBuffer* vbuff, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void set_shader(DX11Shader* shader);
		glm::mat4x4 getProjectionMatrix(float fov_deg);

		virtual void setTargetBackBuffer();
		virtual void setTexture(graphics::Texture* texture, int slot);

		D3D* get_d3d() { return m_d3d; }
		const D3D* get_d3d() const { return m_d3d; }

		void send_data(const DX11ShaderData* data);

	private:

		ConstantBuffer<DX11ShaderDataEntry::buffer_size> m_PS_cbuffs[DX11ShaderData::num_slots] = {};
		ConstantBuffer<DX11ShaderDataEntry::buffer_size> m_VS_cbuffs[DX11ShaderData::num_slots] = {};
		ConstantBuffer<DX11ShaderDataEntry::buffer_size> m_HS_cbuffs[DX11ShaderData::num_slots] = {};
		ConstantBuffer<DX11ShaderDataEntry::buffer_size> m_DS_cbuffs[DX11ShaderData::num_slots] = {};
		ConstantBuffer<DX11ShaderDataEntry::buffer_size> m_GS_cbuffs[DX11ShaderData::num_slots] = {};

		void realloc_d3d();
		D3D* m_d3d = nullptr;
		core::Window* m_window = nullptr;
		DX11Shader* m_shader = nullptr;

		ID3D11SamplerState* m_sampleState = nullptr;

		void init_sampler();
		void release_sampler();

		int m_resolution_width = 10;
		int m_resolution_height = 10;
		bool m_fullscreen = false;
		bool m_vsync = false;
		float m_near_plane = 0.1f;
		float m_far_plane = 1000.f;
	};
}
