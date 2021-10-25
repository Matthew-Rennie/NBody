#pragma once

#include "Input.h"

namespace core
{
	class BaseApplication
	{
	public:

		virtual ~BaseApplication() = default;

		virtual bool Init() { return true; };
		virtual bool Release() { return true; }
		virtual bool Update(const double frame_time) { return true; };
		virtual bool Render() { return true; };
		virtual bool ReadyForRender() { return true; }

	protected:
		Input m_input;
	};
};
