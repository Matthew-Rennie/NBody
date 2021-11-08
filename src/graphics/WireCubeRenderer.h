#include "glm/vec3.hpp"
#include <vector>

namespace dx11
{
	class DX11Renderer;
	class DX11LineRenderer;
}

namespace graphics
{
	class CustomCamera;

	class WireCubeRenderer
	{
	public:

		struct WireCube
		{
			WireCube() :
				position({}), radius(0.0) {}

			WireCube(glm::dvec3 in_position, double in_radius) :
				position(in_position), radius(in_radius) {}

			glm::dvec3 position;
			double radius;
		};

		WireCubeRenderer(dx11::DX11Renderer* renderer, CustomCamera* camera);
		~WireCubeRenderer();

		void AddCube(glm::dvec3 position, double radius);
		void AddCube(WireCube cube);

		void Clear();
		size_t Size() const;
		void Render();

	private:

		void GenerateCubeLines(const WireCube& cube);

		dx11::DX11LineRenderer* m_lineRenderer = nullptr;
		CustomCamera* m_camera = nullptr;

		std::vector<WireCube> m_cubes;
	};
}