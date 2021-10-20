#pragma once

#include "graphics/GraphicsApplication.h"
#include "DX11/DX11VertexBuffer.h"
#include "DX11/DX11Shader.h"
#include "graphics/FreeCamera.h"

class Game : public graphics::GraphicsApplication
{
public:
	Game() = default;
	virtual bool init() override;
	virtual bool update(const float frame_time) override;
	virtual bool render() override;

protected:

	core::BaseShader* m_shader = nullptr;
	dx11::DX11VertexBuffer m_vbuffer;

	void generate_ortho_mesh();
	void generate_terrain_mesh(int resolution);

	graphics::FreeCamera camera;
};