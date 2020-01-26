#pragma once

#include "RenderDevice.h"
#include "TileRenderer.h"

class SvcRender
{
public:
	static bool Create();
	static void Destroy();
	static bool Update();

	static SvcRender* GetInstance();

private:
	SvcRender();
	~SvcRender() {}

	bool Init();
	void Release();

	bool createSwapChain();
	bool createImageViews();
	bool createRenderPass();
	//bool createDescriptorSetLayout();
	bool createFramebuffers();
	bool createCommandBuffers();
	bool createSynchObjects();

	void cleanupSwapChain();
	bool recreateSwapChain();

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

	TileRenderer m_tileRenderer;
};
