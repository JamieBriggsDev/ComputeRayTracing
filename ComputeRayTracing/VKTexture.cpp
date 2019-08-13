#if VK

#include "VKTexture.h"

#include <iostream>

VKTexture::VKTexture(VKModel* _modelRef, VKEngine* _engineRef, const char* _imagePath)
{
	// Keep model and engine reference
	m_vkModelRef = _modelRef;
	m_vkEngineRef = _engineRef;
	// Get image details
	auto details = LoadImage(_imagePath);
	// Bind texture
	CreateTexture(details);
}

void VKTexture::CreateTexture(ImageDetails _details)
{
	// Create buffer
	m_vkModelRef->vkSetupBuffer(m_textureSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		m_vkStagingBuffer,
		m_vkStagingBufferMemory);
	// Copy pixel values to the buffer
	void* data;
	vkMapMemory(*m_vkEngineRef->vkGetDevice(),
		m_vkStagingBufferMemory,
		0,
		m_textureSize,
		0,
		&data);
	memcpy(data, _details.pixels, static_cast<size_t>(m_textureSize));
	vkUnmapMemory(*m_vkEngineRef->vkGetDevice(), m_vkStagingBufferMemory);

	std::cout << "Mapped Texture Memory: Size: " << m_textureSize << std::endl;

}

VKTexture::~VKTexture()
{
}

#endif