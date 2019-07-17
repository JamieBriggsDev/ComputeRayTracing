#if VK

#include "VKEngine.h"
#include "VKObject.h"
#include "VKPipeline.h"
#include "VKModel.h"

#include "Camera.h"


VKObject::VKObject(VKEngine* _vkEngine,
	const char* _modelFilePath)
{
	m_model = new VKModel(_vkEngine);

	m_pipeline = new VKPipeline(_vkEngine->vkGetDevice(), 
		_vkEngine->vkGetSwapChainExtent(), 
		_vkEngine->vkGetSwapChainImageFormat(), 
		"Shaders/vert.spv",
		"Shaders/frag.spv");

	m_modelMatrix = glm::mat4();
}


VKObject::~VKObject()
{

}

void VKObject::Draw(Camera* _camera)
{

}

#endif