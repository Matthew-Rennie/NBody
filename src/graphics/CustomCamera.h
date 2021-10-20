#pragma once
#include "core/Input.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace core { class Window; };

namespace graphics
{
	class CustomCamera
	{
	public:
		CustomCamera();

		void setInput(core::Input* in) { m_input = in; }
		void setWindow(core::Window* window);
		void setFrameTime(float ft) { m_frameTime = ft; }

		void setPosition(float px, float py, float pz);
		void setRotation(float rx, float ry, float rz);

		glm::vec3 getPosition() { return m_position; }
		glm::vec3 getRotation() { return m_rotation; }

		glm::mat4x4 getViewMatrix() { return m_viewMatrix; }
		glm::mat4x4 getOrthoViewMatrix() { return m_orthoMatrix; }

		void update();

	protected:
		glm::vec3 m_position;		///< float3 for position
		glm::vec3 m_rotation;		///< float3 for rotation (angles)
		
		glm::vec3 m_forward;
		glm::vec3 m_up;
		glm::vec3 m_right;

		glm::mat4x4 m_viewMatrix;	///< matrix for current view
		glm::mat4x4 m_orthoMatrix;	///< current orthographic matrix
		float m_frameTime;			///<  time variables
		core::Input* m_input;
		core::Window* m_window;		///< handle to the window
	};
}
