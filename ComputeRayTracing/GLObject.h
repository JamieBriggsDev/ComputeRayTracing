#if GL

#pragma once
#include "Object.h"
class GLObject :
	public IObject
{
public:
	GLObject(const char* _modelFilePath);
	~GLObject();

	void Draw(Camera* _camera);
};

#endif