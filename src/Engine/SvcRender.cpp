#include "SvcRender.h"

#include "SvcLog.h"
#include "SvcWindow.h"
#include "RenderDevice.h"

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

SvcRender::SvcRender()
	: m_renderDevice(nullptr)
	, m_swapchain(nullptr)
	, m_renderPass(nullptr)
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

	if (!createImageViews())
		return false;

	if (!createRenderPass())
		return false;

	if (!createFramebuffers())
		return false;

	if (!createCommandBuffers())
		return false;

	if (!createSynchObjects())
		return false;

	if (!m_tileRenderer.Init(m_renderPass, m_swapchain->extent.width, m_swapchain->extent.height))
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

	m_renderDevice->DestroySwapchain(m_swapchain);
	m_swapchain = nullptr;

	m_renderDevice = nullptr;
	RenderDevice::Destroy();
}

bool SvcRender::createSwapChain()
{
	return m_renderDevice->CreateSwapchain(&m_swapchain);
}

bool SvcRender::createImageViews()
{
	m_swapchainImageViews.resize(m_swapchain->images.size());

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

/*bool SvcRender::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create descriptor set layout!");
		return false;
	}

	return true;
}*/

bool SvcRender::createFramebuffers()
{
	m_swapchainFramebuffers.resize(m_swapchainImageViews.size());

	for (size_t i = 0; i < m_swapchainImageViews.size(); ++i)
	{
		if(!m_renderDevice->CreateFramebuffer(m_swapchainImageViews[i], m_renderPass, m_swapchain->extent.width, m_swapchain->extent.height, &m_swapchainFramebuffers[i]))
			return false;
	}

	return true;
}

bool SvcRender::createCommandBuffers()
{
	m_commandBuffers.resize(m_swapchainFramebuffers.size());
	if (!m_renderDevice->AllocateCommandBuffers(m_commandBuffers.size(), m_commandBuffers.data()))
		return false;

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

	if (!createImageViews())
		return false;

	if (!createRenderPass())
		return false;

	if (!createFramebuffers())
		return false;

	if (!createCommandBuffers())
		return false;

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

	return true;
}

bool SvcRender::Prepare()
{
	m_tileRenderer.Prepare();

	/////

	// Wait
	if (m_imagesInFlight[m_imageIndex] != VK_NULL_HANDLE)
	{
		m_renderDevice->WaitForFences(1, &m_imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
	}
	m_imagesInFlight[m_imageIndex] = m_inFlightFences[m_currentFrame];

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

	m_tileRenderer.Render(m_commandBuffers[m_imageIndex]);

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