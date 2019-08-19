#include "Camera.h"
#include "Window.h"
#include "Controller.h"

Camera::Camera()
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	m_projectionMtx = glm::perspective(glm::radians(45.0f), 
		(float)Window::s_windowWidth / Window::s_windowHeight,
		0.1f, 100.0f);
	// Camera View
	m_viewMtx = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

Camera::~Camera()
{
}

void Camera::ResetLocation()
{
	// Reset position
	m_position = glm::vec3(0, 0, 5);
	// Reset angles
	m_horizontalAngle = 3.14f;
	m_verticalAngle = 0.0f;
	// Reset View Matrix
	m_viewMtx = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

glm::mat4 Camera::GetProjectionView() 
{
	return m_projectionMtx * m_viewMtx;
}

glm::mat4 Camera::GetView()
{
	return m_viewMtx;
}

glm::mat4 Camera::GetProjection()
{
	return m_projectionMtx;
}

void Camera::Update(Window* _window, Controller* _controller, float _deltaTime)
{
	// Get Mouse Position
	glm::dvec2 mousePosition = _controller->GetMousePosition();
	// Get angles from mouse position
	m_horizontalAngle += _controller->GetMouseSpeed() * 
		float(Window::s_windowWidth / 2 - mousePosition.x);
	m_verticalAngle += _controller->GetMouseSpeed() *
		float(Window::s_windowHeight / 2 - mousePosition.y);
	// Stop vertical angle from going too far and upside down
	if (m_verticalAngle > 3.14f / 2.0f)
		m_verticalAngle = 3.14f / 2.0f;
	if (m_verticalAngle < -3.14f / 2.0f)
		m_verticalAngle = -3.14f / 2.0f;

	// Find direction facing using horizontal and vertical angle
	glm::vec3 direction(
		cos(m_verticalAngle) * sin(m_horizontalAngle),
		sin(m_verticalAngle),
		cos(m_verticalAngle) * cos(m_horizontalAngle)
	);

	// Right facing vector
	glm::vec3 right = glm::vec3(
		sin(m_horizontalAngle - 3.14f / 2.0f),
		0,
		cos(m_horizontalAngle - 3.14f / 2.0f)
	);

	// Up facing vector
	glm::vec3 up = glm::cross(right, direction);

	// Take input from controller to move camera backwards and forwards
	if (_controller->IsKeyPressed(_window, GLFW_KEY_W))
	{
		m_position += direction * _deltaTime * m_speed;
	}
	if (_controller->IsKeyPressed(_window, GLFW_KEY_S))
	{
		m_position -= direction * _deltaTime * m_speed;
	}
	// Take input from controller to strafe camera left and right
	if (_controller->IsKeyPressed(_window, GLFW_KEY_D))
	{
		m_position += right * _deltaTime * m_speed;
	}
	if (_controller->IsKeyPressed(_window, GLFW_KEY_A))
	{
		m_position -= right * _deltaTime * m_speed;
	}
	// Take input from controller to float camera up and down
	if (_controller->IsKeyPressed(_window, GLFW_KEY_SPACE))
	{
		m_position += up * _deltaTime * m_speed;
	}
	if (_controller->IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
	{
		m_position -= up * _deltaTime * m_speed;
	}
	// Take input from controller to reset camera
	if (_controller->IsKeyPressed(_window, GLFW_KEY_R))
	{
		ResetLocation();
	}

	// Update View Matrix
	m_viewMtx = glm::lookAt(m_position,
		m_position + direction,
		up);
}

