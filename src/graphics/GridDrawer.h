#pragma once
#include "DX11/DX11Color.h"

namespace dx11
{
	class DX11Renderer;
	class DX11LineRenderer;
}

namespace graphics
{
	class CustomCamera;

	class GridDrawer
	{
	public:

		GridDrawer(dx11::DX11Renderer* renderer, CustomCamera* camera);
		~GridDrawer();
		void Render();

		void SetColor(dx11::Color xAxis, dx11::Color zAxis);

	private:

		dx11::DX11LineRenderer* m_lineRenderer;
		CustomCamera* m_camera;

		dx11::Color m_col_x = CHEESEBOARD_DX11_COLOR_DARKGREEN;
		dx11::Color m_col_z = CHEESEBOARD_DX11_COLOR_DARKRED;
	};
}