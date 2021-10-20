#pragma once

namespace core
{
	class BaseRenderer;

	// generic vertex buffer type
	// all renderers need to implement their own buffer inherited from this one
	class BaseVertexBuffer
	{
	public:

		using IndexType = unsigned long;

		struct VertexType
		{
			float position[3] = {};
			float texture[2] = {};
			float normal[3] = {};
		};

		// standard interface accross all vertex buffer version
		// inherited classes create implementation specific buffers based off this one
		virtual void init(BaseRenderer* renderer, VertexType* vertex_buffer, size_t vertex_count, IndexType* index_buffer, size_t index_count) = 0;

	protected:
	};
}
