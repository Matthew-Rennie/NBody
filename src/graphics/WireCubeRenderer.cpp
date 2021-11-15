#include "stdafx.hpp"
#include "WireCubeRenderer.h"
#include "DX11/DX11LineRenderer.h"
#include "CustomCamera.h"

graphics::WireCubeRenderer::WireCubeRenderer(dx11::DX11Renderer* renderer, CustomCamera* camera) :
	m_camera(camera)
{
	m_lineRenderer = new dx11::DX11LineRenderer(renderer);
}

graphics::WireCubeRenderer::~WireCubeRenderer()
{
	delete m_lineRenderer;
	m_lineRenderer = nullptr;
}

void graphics::WireCubeRenderer::AddCube(glm::dvec3 position, double radius)
{
	m_cubes.emplace_back(position, radius);
}

void graphics::WireCubeRenderer::AddCube(WireCube cube)
{
	m_cubes.push_back(cube);
}

void graphics::WireCubeRenderer::Clear()
{
	m_cubes.clear();
}

size_t graphics::WireCubeRenderer::Size() const
{
	return m_cubes.size();
}

void graphics::WireCubeRenderer::Render()
{
	for (const auto& cube : m_cubes)
	{
		GenerateCubeLines(cube);
	}

	Clear();
	m_lineRenderer->Render(m_camera->getViewMatrix());
}

void graphics::WireCubeRenderer::GenerateCubeLines(const WireCube& cube)
{
	dx11::DX11LineRenderer::Line lines[12] = {};

	lines[0] = { {-cube.radius,-cube.radius,-cube.radius}, {cube.radius,-cube.radius,-cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[1] = { {-cube.radius,-cube.radius,cube.radius}, {cube.radius,-cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[2] = { {-cube.radius,-cube.radius,-cube.radius}, {-cube.radius,-cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[3] = { {cube.radius,-cube.radius,-cube.radius}, {cube.radius,-cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[4] = { {-cube.radius,cube.radius,-cube.radius}, {cube.radius,cube.radius,-cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[5] = { {-cube.radius,cube.radius,cube.radius}, {cube.radius,cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[6] = { {-cube.radius,cube.radius,-cube.radius}, {-cube.radius,cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[7] = { {cube.radius,cube.radius,-cube.radius}, {cube.radius,cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[8] = { {-cube.radius,-cube.radius,-cube.radius}, {-cube.radius,cube.radius,-cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[9] = { {-cube.radius,-cube.radius,cube.radius}, {-cube.radius,cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[10] = { {cube.radius,-cube.radius,-cube.radius}, {cube.radius,cube.radius,-cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };
	lines[11] = { {cube.radius,-cube.radius,cube.radius}, {cube.radius,cube.radius,cube.radius}, CHEESEBOARD_DX11_COLOR_GREEN };

	for (int i = 0; i < 12; i++)
	{
		lines[i].p1 += cube.position;
		lines[i].p2 += cube.position;
		m_lineRenderer->AddLine(lines[i]);
	}
}