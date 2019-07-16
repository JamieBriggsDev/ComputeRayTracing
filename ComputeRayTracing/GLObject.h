#if GL

#pragma once
#include "IObject.h"
class GLObject :
	public IObject
{
public:
	GLObject(const char* _modelFilePath);
	~GLObject();

	void Draw(Camera* _camera);
};

#endif