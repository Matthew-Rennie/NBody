#pragma once
#include "CustomCamera.h"
#include "windows.h"

namespace graphics
{
	class FreeCamera : public CustomCamera
	{
	public:
		void handleInput(double ft);

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

		void setMoveSpeed(double ms) { m_move_speed = ms; }
		void setRotSpeed(double rs) { m_rot_speed = rs; }

		double* getMoveSpeed() { return &m_move_speed; }
		double* getRotSpeed() { return &m_rot_speed; }

	private:
		POINT m_cursor;
		int m_deltax, m_deltay;

		double m_move_speed = 5.0f;
		double m_rot_speed = 0.01f;
	};
}
