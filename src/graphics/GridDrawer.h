#pragma once

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

	private:

		dx11::DX11LineRenderer* m_lineRenderer;
		CustomCamera* m_camera;
	};
}