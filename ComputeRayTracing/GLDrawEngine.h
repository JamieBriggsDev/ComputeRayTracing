#if GL
#pragma once


class Object;
class Camera;
class Window;

class GLDrawEngine
{
public:
	void Update(Camera* _camera,
 
		Window* _window,
 
		Object* _object, float _deltaTime);

	GLDrawEngine();
	~GLDrawEngine();
};

#endif