#pragma once

#include "DX11/DX11VertexBuffer.h"
#include "DX11/DX11Shader.h"
#include "graphics/FreeCamera.h"
#include "core/BaseApplication.h"

namespace graphics
{
	class TextureManager;
}

class Game : public core::BaseApplication
{
public:
	virtual bool init() override;
	virtual bool update(const float frame_time) override;
	virtual bool render() override;

protected:

	core::Window* m_window = nullptr;
	dx11::DX11_Renderer* m_renderer = nullptr;

	dx11::DX11Shader* m_shader = nullptr;
	dx11::DX11VertexBuffer m_vbuffer;

	void generate_ortho_mesh();
	void generate_terrain_mesh(int resolution);

	graphics::FreeCamera camera;
	graphics::TextureManager* m_textureManager;
};