#pragma once

#include "graphics/FreeCamera.h"
#include "core/BaseApplication.h"
#include "Object3d.h"

#include <d3d11.h>
#include <directxmath.h>
#include "DX11/DX11VertexBuffer.h"
#include <array>
using namespace DirectX;

namespace graphics
{
	class TextureManager;
	class WireCubeRenderer;
	class GridDrawer;
}

namespace dx11
{
	class DX11Shader;
	class DX11Renderer;
}

enum class GravitySolverMode
{
	BruteForce_CPU = 0,
	OctTree_CPU = 1,
};

static const std::array<const char*, 2> GravitySolverNames = {
	"Brute Force CPU-Single-Thread",
	"Barnes Hutt CPU-Single-Thread"
};

class GravitySolverBase;
class GravitySolverOctTree;
class GravitySolverBruteForce;

class Game : public core::BaseApplication
{
public:
	bool Init() override;
	bool Release() override;
	bool Update(const double frame_time) override;
	bool Render(const double frame_time) override;

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

	bool m_playing = false;

	core::Window* m_window = nullptr;

	dx11::DX11Renderer* m_renderer = nullptr;
	dx11::DX11Shader* m_shader = nullptr;
	dx11::DX11VertexBuffer<dx11::VertexType>* m_vbuffer = nullptr;

	void InitWindow();

	void GenerateCubeMesh();
	void InitObjects();

	double SystemKineticEnergy();
	double SystemPotentialEnergy();

	void RenderGUI();

	graphics::FreeCamera camera;
	graphics::TextureManager* m_textureManager = nullptr;
	graphics::WireCubeRenderer* m_wireCubeRenderer = nullptr;
	graphics::GridDrawer* m_gridDrawer = nullptr;

	std::vector<Object3d> m_objects;

	GravitySolverMode m_gSolverMode = GravitySolverMode::BruteForce_CPU;
	GravitySolverBruteForce* m_gSolverBruteForce = nullptr;
	GravitySolverOctTree* m_gSolverOctTree = nullptr;

	const double m_timestep = 0.0001f;
};