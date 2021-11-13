#include "glm/vec3.hpp"
#include <directxmath.h>
#include "glm/glm.hpp"
#include <vector>
#include "DX11Color.h"

namespace dx11
{
	class DX11Renderer;
	class DX11Shader;

	class DX11LineRenderer
	{
	public:

		struct Line
		{
			Line() :
				p1({}), p2({}) {}

			Line(glm::dvec3 p1_in, glm::dvec3 p2_in, dx11::Color color_in) :
				p1(p1_in), p2(p2_in), color(color_in) {}

			glm::dvec3 p1;
			glm::dvec3 p2;

			::dx11::Color color;
		};

		DX11LineRenderer(DX11Renderer* renderer);
		~DX11LineRenderer();

		void AddLine(glm::dvec3 p1, glm::dvec3 p2, dx11::Color color);
		void AddLine(Line line);
		void Render(glm::mat4x4 view, bool clearLines = true);
		void Clear();

	private:

		DX11Renderer* m_renderer = nullptr;
		DX11Shader* m_shader = nullptr;

		std::vector<Line> m_lines;

		// DX11VertexBuffer* m_vertexBuffer = nullptr;
	};
}