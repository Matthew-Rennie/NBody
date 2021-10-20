// Input class
// Functions for retrieving input events/state.
#include "stdafx.hpp"
#include "Input.h"

void core::Input::SetKeyDown(WPARAM key)
{
	keys[key] = true;
}

void core::Input::SetKeyUp(WPARAM key)
{
	keys[key] = false;
}

bool core::Input::isKeyDown(int key)
{
	return keys[key];
}

void core::Input::setMouseX(int xPosition)
{
	mouse.x = xPosition;
}

void core::Input::setMouseY(int yPosition)
{
	mouse.y = yPosition;
}

int core::Input::getMouseX()
{
	return mouse.x;
}

int core::Input::getMouseY()
{
	return mouse.y;
}

void core::Input::setLeftMouse(bool down)
{
	mouse.left = down;
}

void core::Input::setRightMouse(bool down)
{
	mouse.right = down;
}

bool core::Input::isLeftMouseDown()
{
	return mouse.left;
}

bool core::Input::isRightMouseDown()
{
	return mouse.right;
}

void core::Input::setMouseActive(bool active)
{
	mouse.isActive = active;
}
bool core::Input::isMouseActive()
{
	return mouse.isActive;
}