#if VK

#include "VKEngine.h"
#include "VKObject.h"
#include "VKPipeline.h"
#include "VKModel.h"
#include "VKTexture.h"

#include "Camera.h"


VKObject::VKObject(VKEngine* _vkEngine,
	const char* _modelFilePath)
{
	m_model = new VKModel(_vkEngine);

	m_texture = new VKTexture(static_cast<VKModel*>(m_model),_vkEngine,"Resources/Textures/Elliot.jpg");

	m_pipeline = new VKPipeline(_vkEngine, 
		_vkEngine->vkGetSwapChainExtent(), 
		_vkEngine->vkGetSwapChainImageFormat(), 
		"Shaders/vert.spv",
		"Shaders/frag.spv",
		this);

	m_modelMatrix = glm::mat4();
}


VKObject::~VKObject()
{

}

void VKObject::Draw(Camera* _camera, float _deltaTime)
{

}

void VKObject::Update(float _deltaTime)
{
}

void VKObject::UpdateUniformBuffer(uint32_t _currentSwapChainImage, float _deltaTime)
{

	static_cast<VKModel*>(m_model)->vkUpdateUniformBuffer(_currentSwapChainImage, _deltaTime);
}

#endif