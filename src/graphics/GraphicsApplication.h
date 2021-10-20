#pragma once

#include "core/BaseApplication.h"
#include "core/BaseRenderer.h"
#include "core/Window.h"
#include "graphics/TextureManager.h"

namespace graphics
{
	class GraphicsApplication : public core::BaseApplication
	{
	public:

		GraphicsApplication();
		~GraphicsApplication();

	protected:

		bool load_renderer(const core::RendererType type);

		core::RendererType m_current_renderer_type = core::RendererType::NONE;
		core::BaseRenderer* m_renderer = nullptr;
		core::Window* m_window = nullptr;

		graphics::TextureManager m_textureManager;
	};
}
