#pragma once

#include "RenderDevice.h"
#include "TileRenderer.h"

#include "glm/vec4.hpp"

class SvcRender
{
public:
	struct GlobalParameters
	{
		glm::vec4	viewportSize;
	};

public:
	static bool Create();
	static void Destroy();
	static bool Update();

	static SvcRender* GetInstance();

	const TileRenderer& GetTileRenderer() const;

private:
	SvcRender();
	~SvcRender() {}

	bool Init();
	void Release();

	bool createSwapChain();
	bool createImageViews();
	bool createRenderPass();
	bool createFramebuffers();
	bool createCommandBuffers();
	bool createGlobalParameters();
	bool createGlobalDescriptorSetLayout();
	bool createGlobalDescriptorPool();
	bool createGlobalDescriptorSets();
	bool createSynchObjects();

	void cleanupSwapChain();
	bool recreateSwapChain();

	bool updateGlobalParameters();

	bool BeginFrame();
	bool Prepare();
	bool Render();
	bool EndFrame();
	bool DrawFrame();

private:
	static SvcRender* ms_instance;

	RenderDevice* m_renderDevice;

	RenderDevice::Swapchain m_swapchain;
	std::vector<RenderDevice::ImageView> m_swapchainImageViews;
	std::vector<RenderDevice::Framebuffer> m_swapchainFramebuffers;

	RenderDevice::RenderPass m_renderPass;
	std::vector<RenderDevice::CommandBuffer> m_commandBuffers;

	std::vector<RenderDevice::Semaphore> m_imageAvailableSemaphores;
	std::vector<RenderDevice::Semaphore> m_renderFinishedSemaphores;
	std::vector<RenderDevice::Fence> m_inFlightFences;
	std::vector<RenderDevice::Fence> m_imagesInFlight;
	size_t m_maxFramesInFlight;
	size_t m_currentFrame;
	uint32_t m_imageIndex;

	std::vector<RenderDevice::Buffer> m_globalParameters;

	RenderDevice::DescriptorSetLayout m_globalDescriptorSetLayout;
	RenderDevice::DescriptorPool m_globalDescriptorPool;
	std::vector<RenderDevice::DescriptorSet> m_globalDescriptorSets;

	TileRenderer m_tileRenderer;
};
