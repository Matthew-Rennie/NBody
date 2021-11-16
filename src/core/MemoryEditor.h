#pragma once

namespace core
{
	class MemoryEditor
	{
	public:

		void Draw(void* const data, size_t size) const;

		template<typename T>
		inline void Draw(T* const object) const
		{
			Draw(object, sizeof(T));
		}
	};
}