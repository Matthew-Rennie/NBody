#include "stdafx.hpp"
#include "sandbox/Game.h"
#include "core/System.h"
#include <dxgi.h>

int main(int argc, char* argv[])
{
	Game* app = new Game();
	core::System* system;

	// Create the system object.
	system = new core::System(app);

	// Initialize and run the system object.
	system->run();

	return 0;
}