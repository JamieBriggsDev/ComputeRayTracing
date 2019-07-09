#pragma once

#include <glm/glm.hpp>

class Window;

class Controller
{
private:
	float m_mouseSpeed = 0.005f;
	glm::dvec2 m_mousePosition;
public:
	bool IsKeyPressed(Window* _window, int _key);
	void Update(Window* _window, float _deltaTime);
	float GetMouseSpeed();
	glm::dvec2 GetMousePosition();
	Controller();
	~Controller();
};

