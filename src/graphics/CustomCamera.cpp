#include "CustomCamera.h"

graphics::CustomCamera::CustomCamera()
{
	m_position = glm::vec3(0.f, 0.f, 0.f);
	m_rotation = glm::vec3(0.f, 0.f, 0.f);

	//lookSpeed = 4.0f;

	// Generate ortho matrix
	glm::vec3 up = { 0.0f, 1.0, 0.0 };
	glm::vec3 position = { 0.0f, 0.0, -10.0 };
	glm::vec3 lookAt = { 0.0f, 0.0f, 0.0f };

	m_orthoMatrix = glm::lookAt(position, lookAt, up);
	//orthoMatrix = XMMatrixLookAtLH(position, lookAt, up);
}

void graphics::CustomCamera::setWindow(core::Window* window)
{
	m_window = window;
}

void graphics::CustomCamera::setPosition(float px, float py, float pz)
{
	m_position.x = px;
	m_position.y = py;
	m_position.z = pz;
}

void graphics::CustomCamera::setRotation(float rx, float ry, float rz)
{
	m_rotation.x = rx;
	m_rotation.y = ry;
	m_rotation.z = rz;
}

void graphics::CustomCamera::update()
{
	glm::mat4x4 rotationMatrix(1);

	// Setup the vectors
	glm::vec3 lookAt = { 0.0, 0.0, 1.0f };

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	float yaw, pitch, roll;
	pitch = m_rotation.x;
	yaw = m_rotation.y;
	roll = m_rotation.z;

	float cosR, cosP, cosY;
	float sinR, sinP, sinY;

	constexpr float pi_by180 = 3.14159265359f / 180.f;

	cosY = cosf(yaw * pi_by180);
	cosP = cosf(pitch * pi_by180);
	cosR = cosf(roll * pi_by180);
	sinY = sinf(yaw * pi_by180);
	sinP = sinf(pitch * pi_by180);
	sinR = sinf(roll * pi_by180);

	m_forward.x = (sinY * cosP);
	m_forward.y = sinP;
	m_forward.z = cosP * -cosY;

	m_up.x = -cosY * sinR - sinY * sinP * cosR;
	m_up.y = cosP * cosR;
	m_up.z = -sinY * sinR - sinP * cosR * -cosY;

	m_right = glm::cross(m_forward, m_up);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = m_position + m_forward;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = glm::lookAt(m_position, lookAt, m_up);
}