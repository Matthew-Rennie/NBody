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
	class DX11LineRenderer;
}

class GravitySolverBase;

class Game : public core::BaseApplication
{
public:
	bool Init() override;
	bool Release() override;
	bool Update(const double frame_time) override;
	bool Render() override;

	bool ReadyForRender() override;

protected:

	double m_fps = 0.f;
	double m_guiUpdateClock = 0.f;
	double m_guiUpdateRate = 0.25f;

	double m_frameClock = 0.0;
	int m_targetFrameRate = 60;
	bool m_enableRendering = true;

	double EnergyK = 0.f;
	double EnergyP = 0.f;
	double EnergyTotal = 0.f;

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	core::Window* m_window = nullptr;

	dx11::DX11Renderer* m_renderer = nullptr;
	dx11::DX11Shader* m_shader = nullptr;
	dx11::DX11VertexBuffer* m_vbuffer = nullptr;
	dx11::DX11LineRenderer* m_lineRenderer = nullptr;

	void InitWindow();

	void GenerateCubeMesh();
	void InitObjects();

	double SystemKineticEnergy();
	double SystemPotentialEnergy();

	void RenderGUI();

	graphics::FreeCamera camera;
	graphics::TextureManager* m_textureManager;

	std::vector<Object3d> m_objects;
	GravitySolverBase* m_gSolver;

	const double m_timestep = 0.0001f;
};