#pragma once

#include "RenderDevice.h"

#include <glm/vec2.hpp>

class TileSetInstance
{
public:
	struct Infos
	{
		glm::vec2 position;
	};
	struct Data
	{
		alignas(16) u32 id;
		alignas(16) glm::vec2 position;
	};

public:
	TileSetInstance();
	bool Init(RenderDevice::DescriptorPool descriptorPool, RenderDevice::DescriptorSetLayout layout);
	void Release();

	size_t count;

	RenderDevice::Buffer infosBuffer;
	RenderDevice::Buffer instanceBuffer;
	RenderDevice::DescriptorSet descriptorSet;
};

class TileSet
{
public:
	struct Parameters
	{
		u32	size;
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
	static const size_t MaxTileSetDescriptorCount = 256;
	static const size_t MaxTileSetInstanceDescriptorCount = 1024;

private:
	RenderDevice::DescriptorSetLayout m_tileSetDescriptorSetLayout;
	RenderDevice::DescriptorSetLayout m_tileSetInstanceDescriptorSetLayout;
	RenderDevice::Pipeline m_pipeline;
	RenderDevice::Buffer m_indexBuffer;

	TileSet m_tileSet;
	TileSetInstance m_tileSetInstance;

	RenderDevice::DescriptorPool m_tileSetDescriptorPool;
	RenderDevice::DescriptorPool m_tileSetInstanceDescriptorPool;
};
