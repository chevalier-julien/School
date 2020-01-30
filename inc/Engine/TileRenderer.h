#pragma once

#include "RenderDevice.h"

class TileSet
{
public:
	struct Parameters
	{
		u32	tileSize;
	};

public:
	TileSet();
	bool Init(RenderDevice::DescriptorPool descriptorPool, RenderDevice::DescriptorSetLayout layout, const char* textureName);
	void Release();

	RenderDevice::Texture texture;
	RenderDevice::Buffer uniformBuffer;
	RenderDevice::DescriptorSet descriptorSet;
};

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

public:
	static const size_t MaxDescriptorCount = 256;

private:
	RenderDevice::DescriptorSetLayout m_tileSetDescriptorSetLayout;
	RenderDevice::Pipeline m_pipeline;
	RenderDevice::Buffer m_indexBuffer;
	TileSet m_tileSet;

	RenderDevice::DescriptorPool m_descriptorPool;
};
