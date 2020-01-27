#pragma once

#include "RenderDevice.h"

class TileRenderer
{
public:
	TileRenderer();
	~TileRenderer();

	bool Init(RenderDevice::RenderPass renderPass, size_t width, size_t height);
	void Release();
	void Prepare();
	void Render(RenderDevice::CommandBuffer commandBuffer);

private:
	RenderDevice::DescriptorSetLayout m_descriptorSetLayout;
	RenderDevice::Pipeline m_pipeline;
	RenderDevice::Buffer m_indexBuffer;
};
