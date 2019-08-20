#if GL

#pragma once
#include "Object.h"
#include "GLModel.h"

class GLObject :
	public Object
{
public:
	GLObject(const char* _modelFilePath);
	~GLObject();

	void Update(float _deltaTime);
};

#endif
