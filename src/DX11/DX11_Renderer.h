#pragma once
#include "core/BaseRenderer.h"
#include <windows.h>
#include "core/Input.h"
#include "D3D.h"

#include <d3d11.h>
#include <directxmath.h>
#include "core/Window.h"
#include "ConstantBuffer.h"

#include "core/BaseShader.h"

namespace dx11
{
	class DX11Shader;

	class DX11_Renderer : public core::BaseRenderer
	{
	public:

		DX11_Renderer(core::Input* in);
		~DX11_Renderer();

		virtual bool init(core::Window* window) override;
		virtual bool release() override;

		virtual void begin_frame() override;
		virtual void end_frame() override;
		virtual void clear_display() override;

		virtual void set_wireframe(bool b) override;
		virtual bool get_wireframe() override;

		virtual void set_vsync(bool b) override;
		virtual bool get_vsync() override;

		virtual void set_resolution(int width, int height) override;

		virtual void render(core::BaseVertexBuffer* vbuff) override;
		virtual void set_shader(core::BaseShader* shader) override;
		glm::mat4x4 getProjectionMatrix(float fov_deg) override;

		virtual void setTargetBackBuffer() override;
		virtual void setTexture(graphics::Texture* texture, int slot) override;

		D3D* get_d3d() { return m_d3d; }
		const D3D* get_d3d() const { return m_d3d; }

	private:

		void send_data(const core::ShaderData* data) override;

		ConstantBuffer<core::ShaderDataEntry::buffer_size> m_PS_cbuffs[core::ShaderData::num_slots] = {};
		ConstantBuffer<core::ShaderDataEntry::buffer_size> m_VS_cbuffs[core::ShaderData::num_slots] = {};
		ConstantBuffer<core::ShaderDataEntry::buffer_size> m_HS_cbuffs[core::ShaderData::num_slots] = {};
		ConstantBuffer<core::ShaderDataEntry::buffer_size> m_DS_cbuffs[core::ShaderData::num_slots] = {};
		ConstantBuffer<core::ShaderDataEntry::buffer_size> m_GS_cbuffs[core::ShaderData::num_slots] = {};

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
