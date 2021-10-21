#pragma once
#include "CustomCamera.h"
#include "windows.h"

namespace graphics
{
	class FreeCamera : public CustomCamera
	{
	public:
		void handleInput(float ft);

		void moveForward();
		void moveBack();
		void moveUp();
		void moveDown();
		void moveLeft();
		void moveRight();

		void turnLeft();
		void turnRight();
		void turnUp();
		void turnDown();
		void turn(int x, int y);

		void setMoveSpeed(float ms) { m_move_speed = ms; }
		void setRotSpeed(float rs) { m_rot_speed = rs; }

		float* getMoveSpeed() { return &m_move_speed; }
		float* getRotSpeed() { return &m_rot_speed; }

	private:
		POINT m_cursor;
		int m_deltax, m_deltay;

		float m_move_speed = 5.0f;
		float m_rot_speed = 100.f;
	};
}
