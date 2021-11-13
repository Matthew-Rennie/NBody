#pragma once

#include "BaseApplication.h"
#include "Timer.h"

namespace core
{
	class System
	{
	public:

		void Run(BaseApplication* app);

	protected:

		core::Timer m_updateTimer;
		core::Timer m_renderTimer;
	};
}
