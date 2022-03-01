#include "stdafx.hpp"
#include "Game.h"
#include "imGUI/imgui.h"
#include "core/Window.h"
#include "core/Timer.h"
#include "graphics/TextureManager.h"
#include "graphics/WireCubeRenderer.h"
#include "graphics/GridDrawer.h"
#include "MeshBuilder.h"
#include "utils.h"

#include "GravitySolverBruteForce.h"
#include "GravitySolverBruteForceMulti.h"
#include "GravitySolverOctTree.h"
#include "GravitySolverOctTreeMulti.h"

#include "DX11/DX11Renderer.h"
#include "DX11/DX11VertexBuffer.h"
#include "DX11/DX11Shader.h"
bool Game::Init()
{
	if (!BaseApplication::Init())
		return false;

	InitWindow();

	m_renderer = new dx11::DX11Renderer(&m_input);
	m_renderer->init(m_window);
	// m_renderer->set_vsync(true);

	m_shader = new dx11::DX11Shader(m_renderer);
	m_shader->load_VS(L"shader_vs.cso", dx11::VertexLayout::VertexTypeLayout, sizeof(dx11::VertexLayout::VertexTypeLayout));
	m_shader->load_PS(L"shader_ps.cso");

	m_vbuffer = new dx11::DX11VertexBuffer<dx11::VertexType>();

	//GenerateTerrainMesh(5);
	m_vbuffer = MeshBuilder::CubeMesh(m_renderer);

	camera.setInput(&m_input);
	camera.setWindow(m_window);

	m_textureManager = new graphics::TextureManager(m_renderer);
	m_textureManager->AddTexture(L"res/bunny.png", "bunny");

	m_wireCubeRenderer = new graphics::WireCubeRenderer(m_renderer, &camera);
	m_gridDrawer = new graphics::GridDrawer(m_renderer, &camera);

	m_gSolverBruteForce = new GravitySolverBruteForce();
	m_gSolverBruteForceMulti = new GravitySolverBruteForceMulti();
	m_gSolverOctTree = new GravitySolverOctTree();
	m_gSolverOctTreeMulti = new GravitySolverOctTreeMulti();

	InitObjectsOrbit();
	return true;
}

bool Game::Release()
{
	if (!BaseApplication::Release())
		return false;

	SAFE_DELETE(m_renderer);
	SAFE_DELETE(m_shader);
	SAFE_DELETE(m_vbuffer);
	SAFE_DELETE(m_window);
	SAFE_DELETE(m_textureManager);
	SAFE_DELETE(m_wireCubeRenderer);
	SAFE_DELETE(m_gridDrawer);
	SAFE_DELETE(m_gSolverBruteForce);
	SAFE_DELETE(m_gSolverBruteForceMulti);
	SAFE_DELETE(m_gSolverOctTree);
	SAFE_DELETE(m_gSolverOctTreeMulti);

	return true;
}

bool Game::Update(const double frame_time)
{
	m_frameClock += frame_time;
	m_guiUpdateClock += frame_time;
	if (m_guiUpdateClock > m_guiUpdateRate)
	{
		m_fps = 1.f / frame_time;
		m_guiUpdateClock = 0;

		// EnergyP = SystemPotentialEnergy();
		// EnergyK = SystemKineticEnergy();
		// EnergyTotal = EnergyK + EnergyP;
	}

	if (!BaseApplication::Update(frame_time))
		return false;

	if (m_input.isKeyDown(VK_ESCAPE))
	{
		return false;
	}

	GravitySolverBase* gSolver = nullptr;
	switch (m_gSolverMode)
	{
	case GravitySolverMode::BRUTE_FORCE_CPU_SINGLE:
		gSolver = m_gSolverBruteForce;
		break;
	case GravitySolverMode::BRUTE_FORCE_CPU_MULTI:
		gSolver = m_gSolverBruteForceMulti;
		break;
	case GravitySolverMode::OCTTREE_CPU_SINGLE:
		gSolver = m_gSolverOctTree;
		break;
	case GravitySolverMode::OCTTREE_CPU_MULTI:
		gSolver = m_gSolverOctTreeMulti;
		break;
	default:
		throw;
	}

	if (m_playing)
	{
		if (gSolver)
			gSolver->CalculateForces(m_objects);

		for (auto& obj : m_objects)
		{
			obj.Update(m_timestep);
		}
	}

	return true;
}

bool Game::Render(const double frame_time)
{
	m_frameClock = 0.0;

	if (!BaseApplication::Render(frame_time))
		return false;

	camera.handleInput(frame_time);
	camera.update();

	struct BuffType
	{
		glm::mat4x4 world;
		glm::mat4x4 view;
		glm::mat4x4 projection;
	};
	static_assert(sizeof(BuffType) <= 512);

	m_renderer->begin_frame();
	m_renderer->setTargetBackBuffer();

	RenderGUI();

	if (m_enableRendering)
	{
		m_renderer->set_shader(m_shader);
		for (auto& obj : m_objects)
		{
			obj.SetVertexBuff(m_vbuffer);
			obj.Render(m_renderer, &camera);
		}
	}

	//auto g_oct = dynamic_cast<GravitySolverOctTree*>(m_gSolver);
	//if (g_oct)
	//{
	//	g_oct->RenderWireframe(m_wireCubeRenderer);
	//}
	m_wireCubeRenderer->Render();
	m_gridDrawer->Render();
	m_renderer->end_frame();

	return true;
}

bool Game::ReadyForRender()
{
	if (BaseApplication::ReadyForRender())
	{
		double t = 1.0 / (double)m_targetFrameRate;
		if (m_frameClock > t)
			return true;
	}

	return false;
}

void Game::InitWindow()
{
	core::Window::WindowConfig config;
	config.borderless = true;
	config.width = 0;
	config.height = 0;
	m_window = new core::Window(&m_input, config);
}

void Game::InitObjectsGrid()
{
	int count_x = 50;
	int count_y = 1;
	int count_z = 50;

	for (int x = 0; x < count_x; x++)
	{
		for (int y = 0; y < count_y; y++)
		{
			for (int z = 0; z < count_z; z++)
			{
				m_objects.emplace_back(m_textureManager);
				m_objects[m_objects.size() - 1].SetPosition({ x,y,z });
				m_objects[m_objects.size() - 1].SetMass(1.f);
			}
		}
	}
	// m_objects[0].SetMass(5.f);
	//
	// long double ld = 0.0l;
	//
	// m_objects.emplace_back(m_textureManager);
	// m_objects.emplace_back(m_textureManager);
	//
	// m_objects[0].SetPosition({ -2,0,0 });
	// m_objects[1].SetPosition({ -1,0,0 });
	// m_objects[0].SetMass(1.f);
	// m_objects[1].SetMass(9.f);
}	//

void Game::InitObjectsOrbit()
{
	const double solar_mass = 100.0;
	const double comet_mass = 1.0;

	m_objects.emplace_back(m_textureManager);
	m_objects[m_objects.size() - 1].SetPosition({ 0,0,0 });
	m_objects[m_objects.size() - 1].SetMass(solar_mass);

	auto spawnCometACW = [&](double radius)
	{
		double velocity = glm::sqrt(solar_mass / radius);

		m_objects.emplace_back(m_textureManager);
		m_objects[m_objects.size() - 1].SetPosition({ radius,0,0 });
		m_objects[m_objects.size() - 1].SetMass(comet_mass);
		m_objects[m_objects.size() - 1].SetVelocity({ 0,0,velocity });

	};

	auto spawnCometCW = [&](double radius)
	{
		double velocity = glm::sqrt(solar_mass / radius);

		m_objects.emplace_back(m_textureManager);
		m_objects[m_objects.size() - 1].SetPosition({ -radius,0,0 });
		m_objects[m_objects.size() - 1].SetMass(comet_mass);
		m_objects[m_objects.size() - 1].SetVelocity({ 0,0,-velocity });

	};

	spawnCometACW(5);
	spawnCometACW(10);
	spawnCometACW(15);
	spawnCometACW(20);
	spawnCometACW(25);

	spawnCometCW(5);
	spawnCometCW(10);
	spawnCometCW(15);
	spawnCometCW(20);
	spawnCometCW(25);

}

void Game::RunTest(int numIterations)
{

	auto testScript = [&](GravitySolverMode gSolverMode) -> void
	{
		GravitySolverBase* gSolver = nullptr;
		std::string log_text;

		switch (gSolverMode)
		{
		case GravitySolverMode::BRUTE_FORCE_CPU_SINGLE:
			log_text = "[gSolverMode = BRUTE_FORCE_CPU_SINGLE] [iterations = {}] [time = {}]";
			gSolver = m_gSolverBruteForce;
			break;
		case GravitySolverMode::BRUTE_FORCE_CPU_MULTI:
			log_text = "[gSolverMode = BRUTE_FORCE_CPU_MULTI ] [iterations = {}] [time = {}]";
			gSolver = m_gSolverBruteForceMulti;
			break;
		case GravitySolverMode::OCTTREE_CPU_SINGLE:
			log_text = "[gSolverMode = OCTTREE_CPU_SINGLE    ] [iterations = {}] [time = {}]";
			gSolver = m_gSolverOctTree;
			break;
		case GravitySolverMode::OCTTREE_CPU_MULTI:
			log_text = "[gSolverMode = OCTTREE_CPU_MULTI     ] [iterations = {}] [time = {}]";
			gSolver = m_gSolverOctTreeMulti;
			break;
		default:
			throw;
		}

		InitObjectsGrid();
		
		core::Timer timer;

		for (int i = 0; i < numIterations; i++)
		{
			gSolver->CalculateForces(m_objects);
			for (auto& obj : m_objects)
			{
				obj.Update(m_timestep);
			}
		}
		timer.frame();

		log_text = utils::format(log_text, numIterations, timer.getTime());
		CB_LOG(log_text);

		CleanupObjects();

	};
	
	CB_LOG("Running Tests");

	testScript(GravitySolverMode::BRUTE_FORCE_CPU_SINGLE);
	testScript(GravitySolverMode::BRUTE_FORCE_CPU_MULTI);
	testScript(GravitySolverMode::OCTTREE_CPU_SINGLE);
	testScript(GravitySolverMode::OCTTREE_CPU_MULTI);

	CB_LOG("Tests Complete");
}

void Game::CleanupObjects()
{
	m_objects.clear();
}

double Game::SystemKineticEnergy()
{
	double e = 0.f;
	for (const auto& obj : m_objects)
	{
		e += obj.KineticEnergy();
	}
	return e / m_timestep;
}

double Game::SystemPotentialEnergy()
{
	double e = 0.f;
	for (int i = 0; i < m_objects.size(); i++)
	{
		for (int j = i + 1; j < m_objects.size(); j++)
		{
			e += m_objects[i].PotentialEnergy(m_objects[j], 1);
		}
	}

	return e;
}

void Game::RenderGUI()
{
	if (m_playing)
	{
		if (ImGui::Button("pause"))
			m_playing = false;
	}
	else
	{
		if (ImGui::Button("Play"))
			m_playing = true;;
	}

	ImGui::Combo("##gSolverSelectorCombo", (int*)(&m_gSolverMode), GravitySolverNames.data(), (int)GravitySolverNames.size());

	if (ImGui::CollapsingHeader("Debug Options"))
	{
		ImGui::Indent(10);

		ImGui::Checkbox("Enable Rendering", &m_enableRendering);
		ImGui::InputInt("Target Frame Rate", &m_targetFrameRate);
		ImGui::InputDouble("GUI Refresh Rate", &m_guiUpdateRate);

		ImGui::Indent(-10);
	}

	if (ImGui::CollapsingHeader("Tests"))
	{
		ImGui::Indent(10);

		if (ImGui::Button("Run Tests"))
			RunTest(100);

		ImGui::Indent(-10);
	}


	if (ImGui::CollapsingHeader("Performance"))
	{
		ImGui::Indent(10);

		ImGui::Text("steps per second: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(m_fps).c_str());

		ImGui::Text("object count: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(m_objects.size()).c_str());

		ImGui::Indent(-10);
	}

	if (ImGui::CollapsingHeader("Energy"))
	{
		ImGui::Indent(10);

		ImGui::Text("Potential Energy: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(EnergyP).c_str());

		ImGui::Text("Kinetic Energy: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(EnergyK).c_str());

		ImGui::Text("Total Energy: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(EnergyTotal).c_str());

		ImGui::Indent(-10);
	}

	if (m_gSolverOctTree && m_gSolverMode == GravitySolverMode::OCTTREE_CPU_SINGLE)
	{
		m_gSolverOctTree->DrawDebugGui();
	}
}