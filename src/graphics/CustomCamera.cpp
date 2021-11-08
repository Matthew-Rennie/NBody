#include "CustomCamera.h"

#include "core/Input.h"

graphics::CustomCamera::CustomCamera()
{
	m_position = glm::dvec3(0.0, 0.0, 0.0);
	m_rotation = glm::dvec3(0.0, 0.0, 0.0);

	//lookSpeed = 4.0f;

	// Generate ortho matrix
	glm::dvec3 up = { 0.0, 1.0, 0.0 };
	glm::dvec3 position = { 0.0, 0.0, -10.0 };
	glm::dvec3 lookAt = { 0.0, 0.0, 0.0 };

	m_orthoMatrix = glm::lookAt(position, lookAt, up);
	//orthoMatrix = XMMatrixLookAtLH(position, lookAt, up);
}

void graphics::CustomCamera::setWindow(core::Window* window)
{
	m_window = window;
}

void graphics::CustomCamera::setPosition(double px, double py, double pz)
{
	m_position.x = px;
	m_position.y = py;
	m_position.z = pz;
}

void graphics::CustomCamera::setRotation(double rx, double ry, double rz)
{
	m_rotation.x = rx;
	m_rotation.y = ry;
	m_rotation.z = rz;
}

void graphics::CustomCamera::update()
{
	glm::dmat4x4 rotationMatrix(1);

	// Setup the vectors
	glm::dvec3 lookAt = { 0.0, 0.0, 1.0 };

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	double yaw, pitch, roll;
	pitch = m_rotation.x;
	yaw = m_rotation.y;
	roll = m_rotation.z;

	double cosR, cosP, cosY;
	double sinR, sinP, sinY;

	constexpr double pi_by180 = 3.14159265359 / 180.0;

	cosY = cos(yaw * pi_by180);
	cosP = cos(pitch * pi_by180);
	cosR = cos(roll * pi_by180);
	sinY = sin(yaw * pi_by180);
	sinP = sin(pitch * pi_by180);
	sinR = sin(roll * pi_by180);

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