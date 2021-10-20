#pragma once

#include "glm/glm.hpp"
//#include "graphics/TextureManager.h"

namespace graphics { struct Texture; };

namespace core
{
	enum class RendererType
	{
		NONE = 0,
		DX11
	};

	class BaseVertexBuffer;
	class Window;
	class BaseShader;
	struct ShaderData;

	class BaseRenderer
	{
	public:

		virtual ~BaseRenderer() = default;

		virtual bool init(Window* window) = 0;
		virtual bool release() = 0;

		virtual void begin_frame() = 0;
		virtual void end_frame() = 0;
		virtual void clear_display() = 0;

		virtual void set_wireframe(bool b) = 0;
		virtual bool get_wireframe() = 0;

		virtual void set_vsync(bool b) = 0;
		virtual bool get_vsync() = 0;

		virtual void set_resolution(int width, int height) = 0;

		// use a dynamic cast to convert to correct type of buffer
		virtual void set_shader(BaseShader* shader) = 0;
		virtual void render(BaseVertexBuffer* vbuff) = 0;
		virtual void send_data(const ShaderData* data) = 0;

		virtual glm::mat4x4 getProjectionMatrix(float fov_deg) = 0;
		// TODO @matthew - set/get fullscreen mode

		virtual void setTargetBackBuffer() = 0;

		virtual void setTexture(graphics::Texture* texture, int slot) = 0;

	protected:
	};
};
