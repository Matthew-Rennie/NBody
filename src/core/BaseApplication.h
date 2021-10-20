#pragma once

#include "Input.h"

namespace core
{
	class BaseApplication
	{
	public:

		virtual ~BaseApplication() = default;

		virtual bool init() { return true; };
		virtual bool update(const float frame_time) { return true; };
		virtual bool render() { return true; };

	protected:
		Input m_input;
	};
};
