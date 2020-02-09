#include "SvcRender.h"

#include "SvcLog.h"
#include "SvcWindow.h"
#include "RenderDevice.h"
#include "DeferredFunc.h"

SvcRender* SvcRender::ms_instance = nullptr;

bool SvcRender::Create()
{
	ms_instance = new SvcRender();

	return ms_instance->Init();
}

void SvcRender::Destroy()
{
	ms_instance->Release();

	delete ms_instance;
	ms_instance = nullptr;
}

bool SvcRender::Update()
{
	return ms_instance->DrawFrame();
}

SvcRender* SvcRender::GetInstance()
{
	return ms_instance;
}

const TileRenderer& SvcRender::GetTileRenderer() const
{
	return m_tileRenderer;
}

SvcRender::SvcRender()
	: m_renderDevice(nullptr)
	, m_swapchain(nullptr)
	, m_renderPass(nullptr)
	, m_globalDescriptorSetLayout(nullptr)
	, m_globalDescriptorPool(nullptr)
	, m_maxFramesInFlight(0)
	, m_currentFrame(0)
	, m_imageIndex(0)
{
}

bool SvcRender::Init()
{
	m_maxFramesInFlight = 2;

	if (!RenderDevice::Create())
		return false;

	m_renderDevice = RenderDevice::GetInstance();

	if (!createSwapChain())
		return false;

	if (!createRenderDeferredContainers())
		return false;

	if (!createImageViews())
		return false;

	if (!createRenderPass())
		return false;

	if (!createFramebuffers())
		return false;

	if (!createCommandBuffers())
		return false;

	if (!createGlobalParameters())
		return false;

	if (!createGlobalDescriptorSetLayout())
		return false;

	if (!createGlobalDescriptorPool())
		return false;

	if (!createGlobalDescriptorSets())
		return false;

	if (!createSynchObjects())
		return false;

	if (!m_tileRenderer.Init(m_renderPass, m_swapchain->extent.width, m_swapchain->extent.height, m_globalDescriptorSetLayout))
		return false;

	return true;
}

void SvcRender::Release()
{
	m_renderDevice->DeviceWaitIdle();

	m_tileRenderer.Release();

	m_imagesInFlight.clear();
	for (size_t i = 0; i < m_maxFramesInFlight; ++i)
	{
		m_renderDevice->DestroyFence(m_inFlightFences[i]);
		m_renderDevice->DestroySemaphore(m_renderFinishedSemaphores[i]);
		m_renderDevice->DestroySemaphore(m_imageAvailableSemaphores[i]);
	}
	m_inFlightFences.clear();
	m_renderFinishedSemaphores.clear();
	m_imageAvailableSemaphores.clear();

	m_globalDescriptorSets.clear();

	m_renderDevice->DestroyDescriptorPool(m_globalDescriptorPool);
	m_globalDescriptorPool = nullptr;

	m_renderDevice->DestroyDescriptorSetLayout(m_globalDescriptorSetLayout);
	m_globalDescriptorSetLayout = nullptr;

	for (RenderDevice::Buffer gParams : m_globalParameters)
	{
		m_renderDevice->DestroyBuffer(gParams);
	}
	m_globalParameters.clear();

	if (!m_commandBuffers.empty())
	{
		m_renderDevice->FreeCommandBuffers(m_commandBuffers.size(), m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	for (RenderDevice::Framebuffer framebuffer : m_swapchainFramebuffers)
	{
		m_renderDevice->DestroyFramebuffer(framebuffer);
	}
	m_swapchainFramebuffers.clear();

	m_renderDevice->DestroyRenderPass(m_renderPass);
	m_renderPass = nullptr;

	for (VkImageView imageView : m_swapchainImageViews)
	{
		m_renderDevice->DestroyImageView(imageView);
	}
	m_swapchainImageViews.clear();

	RenderDeferredHandler_Destroy::Destroy();

	m_renderDevice->DestroySwapchain(m_swapchain);
	m_swapchain = nullptr;

	m_renderDevice = nullptr;
	RenderDevice::Destroy();
}

bool SvcRender::createSwapChain()
{
	return m_renderDevice->CreateSwapchain(&m_swapchain);
}

bool SvcRender::createRenderDeferredContainers()
{
	RenderDeferredHandler_Destroy::Create(m_swapchain->images.size());
	return true;
}

bool SvcRender::createImageViews()
{
	m_swapchainImageViews.resize(m_swapchain->images.size(), nullptr);

	for (size_t i = 0; i < m_swapchain->images.size(); ++i)
	{
		if ( !m_renderDevice->CreateImageView(m_swapchain->images[i], m_swapchain->format, &m_swapchainImageViews[i]))
			return false;
	}

	return true;
}

bool SvcRender::createRenderPass()
{
	return m_renderDevice->CreateRenderPass(m_swapchain->format, &m_renderPass);
}

bool SvcRender::createFramebuffers()
{
	m_swapchainFramebuffers.resize(m_swapchainImageViews.size(), nullptr);

	for (size_t i = 0; i < m_swapchainImageViews.size(); ++i)
	{
		if(!m_renderDevice->CreateFramebuffer(m_swapchainImageViews[i], m_renderPass, m_swapchain->extent.width, m_swapchain->extent.height, &m_swapchainFramebuffers[i]))
			return false;
	}

	return true;
}

bool SvcRender::createCommandBuffers()
{
	m_commandBuffers.resize(m_swapchainFramebuffers.size(), nullptr);
	if (!m_renderDevice->AllocateCommandBuffers(m_commandBuffers.size(), m_commandBuffers.data()))
		return false;

	return true;
}

bool SvcRender::createGlobalParameters()
{
	const size_t bufferSize = sizeof(GlobalParameters);

	m_globalParameters.resize(m_swapchain->images.size(), nullptr);
	for (size_t i = 0; i < m_swapchain->images.size(); ++i)
	{
		if (!m_renderDevice->CreateUniformBuffer(bufferSize,&m_globalParameters[i]))
			return false;
	}

	return true;
}

bool SvcRender::createGlobalDescriptorSetLayout()
{
	const RenderDevice::DescriptorType descriptorTypes[] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	};

	if (!m_renderDevice->CreateDescriptorSetLayout(
		1, descriptorTypes,
		VK_SHADER_STAGE_ALL_GRAPHICS,
		&m_globalDescriptorSetLayout))
		return false;

	return true;
}

bool SvcRender::createGlobalDescriptorPool()
{
	RenderDevice::DescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	if (!m_renderDevice->CreateDescriptorPool(1,&descriptorType, m_swapchain->images.size(), &m_globalDescriptorPool))
		return false;

	return true;
}

bool SvcRender::createGlobalDescriptorSets()
{
	std::vector<RenderDevice::DescriptorSetLayout> layouts(m_swapchain->images.size(), m_globalDescriptorSetLayout);

	m_globalDescriptorSets.resize(m_swapchain->images.size(), nullptr);
	if (!m_renderDevice->AllocateDescriptorSets(m_globalDescriptorPool, m_globalDescriptorSets.size(), layouts.data(), m_globalDescriptorSets.data()))
		return false;

	for (size_t i = 0; i < m_globalDescriptorSets.size(); ++i)
	{
		RenderDevice::DescriptorBufferInfo bufferInfos;
		bufferInfos.buffer = m_globalParameters[i];
		bufferInfos.offset = 0;
		bufferInfos.range = sizeof(GlobalParameters);
		bufferInfos.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		m_renderDevice->UpdateDescriptorSet(m_globalDescriptorSets[i], 1, &bufferInfos, 0, nullptr);
	}

	return true;
}

bool SvcRender::createSynchObjects()
{
	m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
	m_renderFinishedSemaphores.resize(m_maxFramesInFlight);
	m_inFlightFences.resize(m_maxFramesInFlight);
	m_imagesInFlight.resize(m_swapchain->images.size(), nullptr);

	for (size_t i = 0; i < m_maxFramesInFlight; i++)
	{
		if (!m_renderDevice->CreateSemaphore(&m_imageAvailableSemaphores[i]))
			return false;

		if (!m_renderDevice->CreateSemaphore(&m_renderFinishedSemaphores[i]))
			return false;

		if (!m_renderDevice->CreateFence(&m_inFlightFences[i]))
			return false;
	}

	return true;
}

void SvcRender::cleanupSwapChain()
{
	m_tileRenderer.CleanupSwapChain();

	m_globalDescriptorSets.clear();

	m_renderDevice->DestroyDescriptorPool(m_globalDescriptorPool);
	m_globalDescriptorPool = nullptr;

	m_renderDevice->DestroyDescriptorSetLayout(m_globalDescriptorSetLayout);
	m_globalDescriptorSetLayout = nullptr;

	for (RenderDevice::Buffer gParams : m_globalParameters)
	{
		m_renderDevice->DestroyBuffer(gParams);
	}
	m_globalParameters.clear();

	if (!m_commandBuffers.empty())
	{
		m_renderDevice->FreeCommandBuffers(m_commandBuffers.size(), m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	for (RenderDevice::Framebuffer framebuffer : m_swapchainFramebuffers)
	{
		m_renderDevice->DestroyFramebuffer(framebuffer);
	}
	m_swapchainFramebuffers.clear();

	m_renderDevice->DestroyRenderPass(m_renderPass);
	m_renderPass = nullptr;

	for (VkImageView imageView : m_swapchainImageViews)
	{
		m_renderDevice->DestroyImageView(imageView);
	}
	m_swapchainImageViews.clear();

	RenderDeferredHandler_Destroy::Destroy();

	m_renderDevice->DestroySwapchain(m_swapchain);
	m_swapchain = nullptr;
}

bool SvcRender::recreateSwapChain()
{
	int width = 0, height = 0;
	SvcWindow::GetInstance()->GetFramebufferSize(width, height);
	while (width == 0 || height == 0)
	{
		SvcWindow::GetInstance()->GetFramebufferSize(width, height);
		SvcWindow::GetInstance()->WaitEvents();
	}

	m_renderDevice->DeviceWaitIdle();

	cleanupSwapChain();

	if (!createSwapChain())
		return false;

	if (!createRenderDeferredContainers())
		return false;

	if (!createImageViews())
		return false;

	if (!createRenderPass())
		return false;

	if (!createFramebuffers())
		return false;

	if (!createCommandBuffers())
		return false;

	if (!createGlobalParameters())
		return false;

	if (!createGlobalDescriptorSetLayout())
		return false;

	if (!createGlobalDescriptorPool())
		return false;

	if (!createGlobalDescriptorSets())
		return false;

	if (!m_tileRenderer.RecreateSwapChain(m_renderPass, m_swapchain->extent.width, m_swapchain->extent.height, m_globalDescriptorSetLayout))
		return false;

	return true;
}

bool SvcRender::updateGlobalParameters()
{
	RenderDevice::Buffer currentGlobalParameters = m_globalParameters[m_imageIndex];

	float width = static_cast<float>(m_swapchain->extent.width);
	float height = static_cast<float>(m_swapchain->extent.height);

	GlobalParameters gParams;
	gParams.viewportSize = glm::vec4(width, height, 1.0f / width, 1.0f / height);

	void* data;
	if (!m_renderDevice->MapBuffer(currentGlobalParameters, 0, sizeof(gParams), &data))
		return false;

	memcpy(data, &gParams, sizeof(gParams));

	m_renderDevice->UnmapBuffer(currentGlobalParameters);

	return true;
}

bool SvcRender::BeginFrame()
{
	m_renderDevice->WaitForFences(1, &m_inFlightFences[m_currentFrame], true, UINT64_MAX);

	VkResult result = m_renderDevice->AcquireNextImage(m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return true;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to acquire swap chain image!");
		return false;
	}

	RenderDeferredHandler_Destroy::GetInstance()->BeginFrame();

	return true;
}

bool SvcRender::Prepare()
{
	updateGlobalParameters();

	m_tileRenderer.Prepare();

	/////

	// Wait
	if (m_imagesInFlight[m_imageIndex] != VK_NULL_HANDLE)
	{
		m_renderDevice->WaitForFences(1, &m_imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
	}
	m_imagesInFlight[m_imageIndex] = m_inFlightFences[m_currentFrame];

	// deferred destroy
	size_t frameStatusMask = 0;
	for (size_t i = 0; i < m_inFlightFences.size(); ++i)
	{
		if (m_renderDevice->GetFenceStatus(m_inFlightFences[i]) == VK_SUCCESS)
		{
			frameStatusMask |= size_t(1) << i;
		}
	}
	RenderDeferredHandler_Destroy::GetInstance()->DestroyItems(frameStatusMask);

	return true;
}

bool SvcRender::Render()
{
	if (!m_renderDevice->BeginCommandBuffer(m_commandBuffers[m_imageIndex]))
		return false;

	m_renderDevice->BeginRenderPass(
		m_commandBuffers[m_imageIndex],
		m_renderPass,
		m_swapchainFramebuffers[m_imageIndex],
		m_swapchain->extent.width, m_swapchain->extent.height);

	m_tileRenderer.Render(m_commandBuffers[m_imageIndex], m_globalDescriptorSets[m_imageIndex]);

	m_renderDevice->EndRenderPass(m_commandBuffers[m_imageIndex]);

	if (!m_renderDevice->EndCommandBuffer(m_commandBuffers[m_imageIndex]))
		return false;

	return true;
}

bool SvcRender::EndFrame()
{
	m_renderDevice->ResetFences(1, &m_inFlightFences[m_currentFrame]);

	if (!m_renderDevice->QueueSubmit(1, &m_commandBuffers[m_imageIndex],
		m_imageAvailableSemaphores[m_currentFrame],
		m_renderFinishedSemaphores[m_currentFrame],
		m_inFlightFences[m_currentFrame]))
		return false;

	RenderDevice::Result result = m_renderDevice->QueuePresent(m_swapchain, m_renderFinishedSemaphores[m_currentFrame], m_imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to present swap chain image!");
		return false;
	}

	m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;

	return true;
}

bool SvcRender::DrawFrame()
{
	if (!BeginFrame())
		return false;

	if (!Prepare())
		return false;

	if (!Render())
		return false;

	if (!EndFrame())
		return false;

	return true;
}
