#include "stdafx.hpp"
#include "GraphicsApplication.h"

#include "DX11/DX11_Renderer.h"

graphics::GraphicsApplication::GraphicsApplication()
{
	m_window = new core::Window(&m_input);

	core::Window::WindowConfig config;
	config.borderless = true;
	config.width = 0;
	config.height = 0;

	m_window->init(config);
}

graphics::GraphicsApplication::~GraphicsApplication()
{
	delete m_window;
	m_window = nullptr;
}

bool graphics::GraphicsApplication::load_renderer(const core::RendererType type)
{
	if (type == m_current_renderer_type)
	{
		return true;
	}

	if (m_renderer)
	{
		m_renderer->release();
		delete m_renderer;
		m_renderer = nullptr;
	}

	switch (type)
	{
	case core::RendererType::NONE:
		m_textureManager.SetRenderer(nullptr);
		return true;
	case core::RendererType::DX11:
		m_renderer = new dx11::DX11_Renderer(&m_input);
		break;
	default:
		return false;
	}

	bool result = m_renderer->init(m_window);
	m_textureManager.SetRenderer(m_renderer);

	return result;
}