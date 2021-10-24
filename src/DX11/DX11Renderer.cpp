#include "stdafx.hpp"
#include "DX11Renderer.h"
#include "DX11VertexBuffer.h"
#include "DX11Shader.h"
#include <stdexcept>
#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_dx11.h"
#include "imGUI/imgui_impl_win32.h"
#include "graphics/TextureManager.h"

dx11::DX11Renderer::DX11Renderer(core::Input* in)
{
}

dx11::DX11Renderer::~DX11Renderer()
{
	release();
}

bool dx11::DX11Renderer::init(core::Window* window)
{
	m_window = window;

	// TODO @matthew - this size should be controlled by engine not windows
	m_resolution_width = GetSystemMetrics(SM_CXSCREEN);
	m_resolution_height = GetSystemMetrics(SM_CYSCREEN);

	m_d3d = new D3D(m_resolution_width, m_resolution_height, m_window->hwnd(), m_fullscreen, m_far_plane, m_near_plane);
	//m_d3d->setWireframeMode(true);

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(m_window->hwnd());
	ImGui_ImplDX11_Init(m_d3d->getDevice(), m_d3d->getDeviceContext());

	for (int slot = 0; slot < dx11::DX11ShaderData::num_slots; slot++)
	{
		m_VS_cbuffs[slot].initBuffer(m_d3d->getDevice());
		m_PS_cbuffs[slot].initBuffer(m_d3d->getDevice());
		m_HS_cbuffs[slot].initBuffer(m_d3d->getDevice());
		m_DS_cbuffs[slot].initBuffer(m_d3d->getDevice());
		m_GS_cbuffs[slot].initBuffer(m_d3d->getDevice());

		m_VS_cbuffs[slot].startSlot() = slot;
		m_PS_cbuffs[slot].startSlot() = slot;
		m_HS_cbuffs[slot].startSlot() = slot;
		m_DS_cbuffs[slot].startSlot() = slot;
		m_GS_cbuffs[slot].startSlot() = slot;
	}
	init_sampler();
	return true;
}

bool dx11::DX11Renderer::release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	delete m_d3d;
	m_d3d = nullptr;

	release_sampler();

	return true;
}

void dx11::DX11Renderer::begin_frame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	clear_display();
}

void dx11::DX11Renderer::end_frame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	m_d3d->endScene(m_vsync);
}

void dx11::DX11Renderer::clear_display()
{
	auto context = m_d3d->getDeviceContext();
	float color[4] = { 0.39f, 0.58f, 0.92f, 1.0f };

	context->ClearRenderTargetView(m_d3d->RenderTargetView(), color);
	context->ClearDepthStencilView(m_d3d->DepthTargetView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void dx11::DX11Renderer::set_wireframe(bool b)
{
	m_d3d->setWireframeMode(b);
}

bool dx11::DX11Renderer::get_wireframe()
{
	return m_d3d->getWireframeState();
}

void dx11::DX11Renderer::set_vsync(bool b)
{
	m_vsync = b;
}

bool dx11::DX11Renderer::get_vsync()
{
	return m_vsync;
}

void dx11::DX11Renderer::set_resolution(int width, int height)
{
	// TODO @matthew - only need to reallocate the swapchan not whole of d3d
	if (m_resolution_width != width || m_resolution_width != height)
	{
		m_resolution_width = width;
		m_resolution_width = height;
		realloc_d3d();
	}
}

void dx11::DX11Renderer::render(dx11::DX11VertexBuffer* vbuff)
{
	UINT stride = (UINT)sizeof(dx11::DX11VertexBuffer::DX11VertexType);
	UINT offset = 0;

	// Set vertex buffer stride and offset.

	m_d3d->getDeviceContext()->IASetVertexBuffers(0, 1, vbuff->pVertexBuffer(), &stride, &offset);
	m_d3d->getDeviceContext()->IASetIndexBuffer(vbuff->IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_d3d->getDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d->getDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	m_shader->run(m_d3d->getDeviceContext(), (UINT)vbuff->IndexCount());
}

void dx11::DX11Renderer::set_shader(DX11Shader* shader)
{
	m_shader = shader;
}

glm::mat4x4 dx11::DX11Renderer::getProjectionMatrix(float fov_deg)
{
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov_deg * 0.0174533f, (float)m_resolution_width / (float)m_resolution_height, m_near_plane, m_far_plane);

	DirectX::XMFLOAT4X4 dx_proj;
	DirectX::XMStoreFloat4x4(&dx_proj, projectionMatrix);
	glm::mat4x4 proj;

	static_assert(sizeof(proj) == sizeof(dx_proj));
	memcpy(&proj, &dx_proj, sizeof(proj));
	return proj;
}

void dx11::DX11Renderer::setTargetBackBuffer()
{
	m_d3d->setBackBufferRenderTarget();
	m_d3d->resetViewport();
}

void dx11::DX11Renderer::setTexture(graphics::Texture* texture, int slot)
{
	m_d3d->getDeviceContext()->PSSetShaderResources(slot, 1, &texture->dx11_texture);
}

void dx11::DX11Renderer::send_data(const dx11::DX11ShaderData* data)
{
	if (!m_shader)
		return;

	auto sendPS = [&](int start_slot) -> void
	{
		auto cbuff = &m_PS_cbuffs[start_slot];
		auto cdata = &data->ps_data[start_slot];

		if (!cdata->enable)
			return;

		cbuff->map(m_d3d->getDeviceContext());
		{
			void* cbuff_data = cbuff->mappedResource();
			memcpy(cbuff_data, cdata->buffer, cdata->buffer_size);
		}
		cbuff->unmap(m_d3d->getDeviceContext());
		cbuff->setBuffer_PS(m_d3d->getDeviceContext());
	};

	auto sendVS = [&](int start_slot) -> void
	{
		auto cbuff = &m_VS_cbuffs[start_slot];
		auto cdata = &data->vs_data[start_slot];

		if (!cdata->enable)
			return;

		cbuff->map(m_d3d->getDeviceContext());
		{
			void* cbuff_data = cbuff->mappedResource();
			memcpy(cbuff_data, cdata->buffer, cdata->buffer_size);
		}
		cbuff->unmap(m_d3d->getDeviceContext());
		cbuff->setBuffer_VS(m_d3d->getDeviceContext());
	};

	auto sendHS = [&](int start_slot) -> void
	{
		auto cbuff = &m_HS_cbuffs[start_slot];
		auto cdata = &data->hs_data[start_slot];

		if (!cdata->enable)
			return;

		cbuff->map(m_d3d->getDeviceContext());
		{
			void* cbuff_data = cbuff->mappedResource();
			memcpy(cbuff_data, cdata->buffer, cdata->buffer_size);
		}
		cbuff->unmap(m_d3d->getDeviceContext());
		cbuff->setBuffer_HS(m_d3d->getDeviceContext());
	};

	auto sendDS = [&](int start_slot) -> void
	{
		auto cbuff = &m_DS_cbuffs[start_slot];
		auto cdata = &data->ds_data[start_slot];

		if (!cdata->enable)
			return;

		cbuff->map(m_d3d->getDeviceContext());
		{
			void* cbuff_data = cbuff->mappedResource();
			memcpy(cbuff_data, cdata->buffer, cdata->buffer_size);
		}
		cbuff->unmap(m_d3d->getDeviceContext());
		cbuff->setBuffer_DS(m_d3d->getDeviceContext());
	};

	auto sendGS = [&](int start_slot) -> void
	{
		auto cbuff = &m_GS_cbuffs[start_slot];
		auto cdata = &data->gs_data[start_slot];

		if (!cdata->enable)
			return;

		cbuff->map(m_d3d->getDeviceContext());
		{
			void* cbuff_data = cbuff->mappedResource();
			memcpy(cbuff_data, cdata->buffer, cdata->buffer_size);
		}
		cbuff->unmap(m_d3d->getDeviceContext());
		cbuff->setBuffer_GS(m_d3d->getDeviceContext());
	};

	for (int slot = 0; slot < dx11::DX11ShaderData::num_slots; slot++)
	{
		sendPS(slot);
		sendVS(slot);
		sendHS(slot);
		sendDS(slot);
		sendGS(slot);
	}
}

void dx11::DX11Renderer::realloc_d3d()
{
	ImGui_ImplDX11_Shutdown();
	release_sampler();

	delete m_d3d;
	m_d3d = nullptr;
	m_d3d = new D3D(m_resolution_width, m_resolution_height, m_window->hwnd(), m_fullscreen, m_far_plane, m_near_plane);

	init_sampler();
	ImGui_ImplDX11_Init(m_d3d->getDevice(), m_d3d->getDeviceContext());
}

void dx11::DX11Renderer::init_sampler()
{
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		m_d3d->getDevice()->CreateSamplerState(&samplerDesc, &m_sampleState);
	}
}

void dx11::DX11Renderer::release_sampler()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}
}