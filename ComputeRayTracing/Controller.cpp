#include "Controller.h"
#include "Window.h"

bool Controller::IsKeyPressed(Window* _window, int _key)
{
	if (glfwGetKey(_window->GetWindowComponent(), _key) == GLFW_PRESS) 
	{
		return true;
	}

	return false;
}

void Controller::Update(Window* _window, float _deltaTime)
{
	//if(_window->GetWindowComponent()->)
	// Get mouse position
	glfwGetCursorPos(_window->GetWindowComponent(), 
		&m_mousePosition.x, 
		&m_mousePosition.y);

	// Reset mouse position to middle of window
	glfwSetCursorPos(_window->GetWindowComponent(), 
		Window::s_windowWidth / 2, 
		Window::s_windowHeight / 2);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(_window->GetWindowComponent(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

float Controller::GetMouseSpeed()
{
	return m_mouseSpeed;
}

glm::dvec2 Controller::GetMousePosition()
{
	return m_mousePosition;
}

Controller::Controller()
{
}


Controller::~Controller()
{
}
