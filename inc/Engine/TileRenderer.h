#pragma once

#include "RenderDevice.h"

class TileSetInstance;

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

	RenderDevice::DescriptorSetLayout GetTileSetInstanceDescriptorSetLayout() const;
	RenderDevice::DescriptorPool GetTileSetInstanceDescriptorPool() const;

public:
	static const size_t MaxTileSetInstanceDescriptorCount = 1024;

private:
	RenderDevice::DescriptorPool m_tileSetInstanceDescriptorPool;

	RenderDevice::DescriptorSetLayout m_tileSetInstanceDescriptorSetLayout;
	RenderDevice::Pipeline m_pipeline;
	RenderDevice::Buffer m_indexBuffer;

	typedef std::vector<const TileSetInstance*> TileMap;
	TileMap m_tileMap;
};
