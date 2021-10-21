#include "stdafx.hpp"
#include "sandbox/Game.h"
#include "core/System.h"
#include <dxgi.h>

int main(int argc, char* argv[])
{
	auto app = new Game();
	core::System system;

	// Initialize and run the system object.
	system.Run(app);

	delete app;
	app = nullptr;

	return 0;
}