#include "glm/vec3.hpp"
#include <directxmath.h>
#include "glm/glm.hpp"
#include <vector>

namespace dx11
{
	class DX11Renderer;
	class DX11Shader;

	class DX11LineRenderer
	{
	public:

		struct Line
		{
			Line(glm::dvec3 p1_in, glm::dvec3 p2_in) :
				p1(p1_in), p2(p2_in) {}

			glm::dvec3 p1;
			glm::dvec3 p2;
		};

		DX11LineRenderer(DX11Renderer* renderer);
		~DX11LineRenderer();

		void Draw(glm::dvec3 p1, glm::dvec3 p2, glm::mat4x4 view);

		void AddLine(glm::dvec3 p1, glm::dvec3 p2);
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