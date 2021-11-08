#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace core
{
	class Window;
	class Input;
};

namespace graphics
{
	class CustomCamera
	{
	public:
		CustomCamera();

		void setInput(core::Input* in) { m_input = in; }
		void setWindow(core::Window* window);
		void setFrameTime(double ft) { m_frameTime = ft; }

		void setPosition(double px, double py, double pz);
		void setRotation(double rx, double ry, double rz);

		glm::dvec3 getPosition() { return m_position; }
		glm::dvec3 getRotation() { return m_rotation; }

		glm::dmat4x4 getViewMatrix() { return m_viewMatrix; }
		glm::dmat4x4 getOrthoViewMatrix() { return m_orthoMatrix; }

		void update();

	protected:
		glm::dvec3 m_position;		///< float3 for position
		glm::dvec3 m_rotation;		///< float3 for rotation (angles)

		glm::dvec3 m_forward;
		glm::dvec3 m_up;
		glm::dvec3 m_right;

		glm::dmat4x4 m_viewMatrix;	///< matrix for current view
		glm::dmat4x4 m_orthoMatrix;	///< current orthographic matrix
		double m_frameTime;			///<  time variables
		core::Input* m_input;
		core::Window* m_window;		///< handle to the window
	};
}
