#pragma once

#include "graphics/FreeCamera.h"
#include "core/BaseApplication.h"
#include "Object3d.h"

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

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

class GravitySolverBase;

class Game : public core::BaseApplication
{
public:
	bool Init() override;
	bool Release() override;
	bool Update(const float frame_time) override;
	bool Render() override;

protected:

	float m_fps = 0.f;
	float t = 0.f;

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	core::Window* m_window = nullptr;

	dx11::DX11Renderer* m_renderer = nullptr;
	dx11::DX11Shader* m_shader = nullptr;
	dx11::DX11VertexBuffer* m_vbuffer;

	void InitWindow();

	void GenerateTerrainMesh(int resolution);
	void GenerateCubeMesh();
	void InitObjects();

	graphics::FreeCamera camera;
	graphics::TextureManager* m_textureManager;

	std::vector<Object3d> m_objects;
	GravitySolverBase* m_gSolver;
};