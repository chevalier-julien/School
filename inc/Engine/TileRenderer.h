#pragma once

#include "RenderDevice.h"

class TileRenderer
{
public:
	TileRenderer();
	~TileRenderer();

	bool Init(RenderDevice::RenderPass renderPass, size_t viewportWidth, size_t viewportHeight, RenderDevice::DescriptorSetLayout globalDescriptorSetLayout);
	void Release();
	void Prepare();
	void Render(RenderDevice::CommandBuffer commandBuffer, RenderDevice::DescriptorSet globalDescriptorSet);

	void CleanupSwapChain();
	bool RecreateSwapChain(RenderDevice::RenderPass renderPass, size_t viewportWidth, size_t viewportHeight, RenderDevice::DescriptorSetLayout globalDescriptorSetLayout);

private:
	RenderDevice::DescriptorSetLayout m_descriptorSetLayout;
	RenderDevice::Pipeline m_pipeline;
	RenderDevice::Buffer m_indexBuffer;
	RenderDevice::Texture m_texture;

	RenderDevice::DescriptorPool m_descriptorPool;
	std::vector<RenderDevice::DescriptorSet> m_descriptorSets;
};
