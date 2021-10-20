#pragma once

#include "BaseApplication.h"
#include "Timer.h"

namespace core
{
	class System
	{
	public:

		System(BaseApplication* app);

		void run();

	protected:

		core::Timer m_timer;
		BaseApplication* m_app = nullptr;
	};
}
