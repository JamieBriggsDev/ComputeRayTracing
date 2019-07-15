#pragma once


class IObject;
class Camera;
class Window;

class DrawEngine
{
public:
	void Update(Camera* _camera, 
		Window* _window, 
		IObject* _object);

	DrawEngine();
	~DrawEngine();
};

