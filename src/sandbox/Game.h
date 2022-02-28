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
	BRUTE_FORCE_CPU_SINGLE = 0,
	BRUTE_FORCE_CPU_MULTI = 1,
	OCTTREE_CPU_SINGLE = 2,
	OCTTREE_CPU_MULTI = 3,
};

static const std::array<const char*, 4> GravitySolverNames = {
	"Brute Force CPU-Single-Thread",
	"Brute Force CPU-Multi-Thread",
	"Barnes Hutt CPU-Single-Thread",
	"Barnes Hutt CPU-Multi-Thread",
};

class GravitySolverBase;
class GravitySolverOctTree;
class GravitySolverOctTreeMulti;
class GravitySolverBruteForce;
class GravitySolverBruteForceMulti;

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

	void InitObjectsGrid();
	void InitObjectsOrbit();

	void RunTest(int numIterations);

	void CleanupObjects();

	double SystemKineticEnergy();
	double SystemPotentialEnergy();

	void RenderGUI();

	graphics::FreeCamera camera;
	graphics::TextureManager* m_textureManager = nullptr;
	graphics::WireCubeRenderer* m_wireCubeRenderer = nullptr;
	graphics::GridDrawer* m_gridDrawer = nullptr;

	std::vector<Object3d> m_objects;

	GravitySolverMode m_gSolverMode = GravitySolverMode::BRUTE_FORCE_CPU_SINGLE;
	GravitySolverBruteForce* m_gSolverBruteForce = nullptr;
	GravitySolverBruteForceMulti* m_gSolverBruteForceMulti = nullptr;
	GravitySolverOctTree* m_gSolverOctTree = nullptr;
	GravitySolverOctTreeMulti* m_gSolverOctTreeMulti = nullptr;

	const double m_timestep = 0.0001f;
};