#include "FreeCamera.h"
#include "core/Window.h"

void graphics::FreeCamera::handleInput(float dt)
{
	setFrameTime(dt);
	// Handle the input.
	if (m_input->isKeyDown('W'))
	{
		// forward
		moveForward();
	}
	if (m_input->isKeyDown('S'))
	{
		// back
		moveBack();
	}
	if (m_input->isKeyDown('A'))
	{
		// Strafe Left
		moveLeft();
	}
	if (m_input->isKeyDown('D'))
	{
		// Strafe Right
		moveRight();
	}
	if (m_input->isKeyDown('Q'))
	{
		// Down
		moveDown();
	}
	if (m_input->isKeyDown(VK_SPACE))
	{
		// Up
		moveUp();
	}
	if (m_input->isKeyDown(VK_UP))
	{
		// rotate up
		turnUp();
	}
	if (m_input->isKeyDown(VK_DOWN))
	{
		// rotate down
		turnDown();
	}
	if (m_input->isKeyDown(VK_LEFT))
	{
		// rotate left
		turnLeft();
	}
	if (m_input->isKeyDown(VK_RIGHT))
	{
		// rotate right
		turnRight();
	}

	if (m_input->isMouseActive())
	{
		// mouse look is on
		m_deltax = m_input->getMouseX() - (m_window->screen_width() / 2);
		m_deltay = m_input->getMouseY() - (m_window->screen_height() / 2);
		turn(m_deltax, m_deltay);
		m_cursor.x = m_window->screen_width() / 2;
		m_cursor.y = m_window->screen_height() / 2;
		ClientToScreen(m_window->hwnd(), &m_cursor);
		SetCursorPos(m_cursor.x, m_cursor.y);
	}

	if (m_input->isRightMouseDown() && !m_input->isMouseActive())
	{
		// re-position cursor
		m_cursor.x = m_window->screen_width() / 2;
		m_cursor.y = m_window->screen_height() / 2;
		ClientToScreen(m_window->hwnd(), &m_cursor);
		SetCursorPos(m_cursor.x, m_cursor.y);
		m_input->setMouseX(m_window->screen_width() / 2);
		m_input->setMouseY(m_window->screen_height() / 2);

		// set mouse tracking as active and hide mouse cursor
		m_input->setMouseActive(true);
		ShowCursor(false);
	}
	else if (!m_input->isRightMouseDown() && m_input->isMouseActive())
	{
		// disable mouse tracking and show mouse cursor
		m_input->setMouseActive(false);
		ShowCursor(true);
	}

	//if (input->isKeyDown(VK_SPACE))
	//{
	//	// re-position cursor
	//	cursor.x = winWidth / 2;
	//	cursor.y = winHeight / 2;
	//	ClientToScreen(wnd, &cursor);
	//	SetCursorPos(cursor.x, cursor.y);
	//	input->setMouseX(winWidth / 2);
	//	input->setMouseY(winHeight / 2);
	//	input->SetKeyUp(VK_SPACE);
	//	// if space pressed toggle mouse
	//	input->setMouseActive(!input->isMouseActive());
	//	if (!input->isMouseActive())
	//	{
	//		ShowCursor(true);
	//	}
	//	else
	//	{
	//		ShowCursor(false);
	//	}
	//}
	update();
}

void graphics::FreeCamera::moveForward()
{
	// Update the forward movement based on the frame time
	float speed = m_frameTime * m_move_speed;

	// Update the position.
	m_position -= m_forward * speed;
}

void graphics::FreeCamera::moveBack()
{
	// Update the backward movement based on the frame time
	float speed = m_frameTime * m_move_speed;// *0.5f;

	// Update the position.
	m_position += m_forward * speed;
}

void graphics::FreeCamera::moveUp()
{
	// Update the upward movement based on the frame time
	float speed = m_frameTime * m_move_speed;// *0.5f;

	m_position += m_up * speed;
}

void graphics::FreeCamera::moveDown()
{
	// Update the downward movement based on the frame time
	float speed = m_frameTime * m_move_speed;// *0.5f;

	m_position -= m_up * speed;
}

void graphics::FreeCamera::moveLeft()
{
	// Update the forward movement based on the frame time
	float speed = m_frameTime * m_move_speed;

	m_position -= m_right * speed;
}

void graphics::FreeCamera::moveRight()
{
	// Update the forward movement based on the frame time
	float speed = m_frameTime * m_move_speed;

	m_position += m_right * speed;
}

void graphics::FreeCamera::turnLeft()
{
	// Update the left turn movement based on the frame time
	float speed = m_frameTime * m_rot_speed;

	// Update the rotation.
	m_rotation.y += speed;

	// Keep the rotation in the 0 to 360 range.
	if (m_rotation.y < 0.0f)
	{
		m_rotation.y += 360.0f;
	}
}

void graphics::FreeCamera::turnRight()
{
	// Update the right turn movement based on the frame time
	float speed = m_frameTime * m_rot_speed;

	// Update the rotation.
	m_rotation.y -= speed;

	// Keep the rotation in the 0 to 360 range.
	if (m_rotation.y > 360.0f)
	{
		m_rotation.y -= 360.0f;
	}
}

void graphics::FreeCamera::turnUp()
{
	// Update the upward rotation movement based on the frame time
	float speed = m_frameTime * m_rot_speed;

	// Update the rotation.
	m_rotation.x -= speed;

	// Keep the rotation maximum 90 degrees.
	if (m_rotation.x > 90.0f)
	{
		m_rotation.x = 90.0f;
	}
}

void graphics::FreeCamera::turnDown()
{
	// Update the downward rotation movement based on the frame time
	float speed = m_frameTime * m_rot_speed;

	// Update the rotation.
	m_rotation.x += speed;

	// Keep the rotation maximum 90 degrees.
	if (m_rotation.x < -90.0f)
	{
		m_rotation.x = -90.0f;
	}
}

void graphics::FreeCamera::turn(int x, int y)
{
	// Update the rotation.
	m_rotation.y += (float)x / (m_rot_speed / 5.0f);// m_speed * x;

	m_rotation.x += (float)y / (m_rot_speed / 5.0f);// m_speed * y;
}