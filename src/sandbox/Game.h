#pragma once

#include "graphics/FreeCamera.h"
#include "core/BaseApplication.h"

namespace graphics
{
	class TextureManager;
}

namespace dx11
{
	class DX11Shader;
	class DX11VertexBuffer;
	class DX11Renderer;
}

class Game : public core::BaseApplication
{
public:
	bool Init() override;
	bool Release() override;
	bool Update(const float frame_time) override;
	bool Render() override;

protected:

	core::Window* m_window = nullptr;

	dx11::DX11Renderer* m_renderer = nullptr;
	dx11::DX11Shader* m_shader = nullptr;
	dx11::DX11VertexBuffer* m_vbuffer;

	void InitWindow();

	void GenerateTerrainMesh(int resolution);

	graphics::FreeCamera camera;
	graphics::TextureManager* m_textureManager;
};