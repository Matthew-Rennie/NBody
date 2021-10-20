#pragma once

#include "BaseRenderer.h"
#include "Input.h"

namespace core
{
	class BaseApplication
	{
	public:

		virtual ~BaseApplication() = default;

		virtual bool init() = 0;
		virtual bool update(const float frame_time) = 0;
		virtual bool render() = 0;

	protected:
		Input m_input;
	};
};
