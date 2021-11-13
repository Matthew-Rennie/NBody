#include "stdafx.hpp"
#include "GridDrawer.h"
#include "dx11/DX11LineRenderer.h"
#include "CustomCamera.h"

graphics::GridDrawer::GridDrawer(dx11::DX11Renderer* renderer, CustomCamera* camera) :
	m_camera(camera)
{
	m_lineRenderer = new dx11::DX11LineRenderer(renderer);
}

graphics::GridDrawer::~GridDrawer()
{
	delete m_lineRenderer;
	m_lineRenderer = nullptr;
}

void graphics::GridDrawer::Render()
{
	const int start = -20;
	const int end = 20;

	const float bigfloat = 20.f;

	for (int x = start; x <= end; x++)
	{
		m_lineRenderer->AddLine({ float(x), 0.f, -bigfloat }, { float(x), 0.f, bigfloat }, CHEESEBOARD_DX11_LINE_COLOR_DARKGREEN);
	}

	for (int z = start; z <= end; z++)
	{
		m_lineRenderer->AddLine({ -bigfloat, 0.f, float(z) }, { bigfloat, 0.f, float(z) }, CHEESEBOARD_DX11_LINE_COLOR_DARKRED);
	}

	m_lineRenderer->Render(m_camera->getViewMatrix());
}