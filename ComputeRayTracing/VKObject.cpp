#if VK

#include "VKObject.h"
#include "Camera.h"
#include <vulkan/vulkan.h>


VKObject::VKObject(VkDevice _vkDevice, const char* _modelFilePath)
{
	m_model = new Model(_modelFilePath);

	m_shader = new Shader(_vkDevice, "Shaders/vert.spv",
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