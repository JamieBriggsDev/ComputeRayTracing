#pragma once


class Object;
class Camera;
class Window;

class DrawEngine
{
public:
	void Update(Camera* _camera, Window* _window, Object* _object);

	DrawEngine();
	~DrawEngine();
};

