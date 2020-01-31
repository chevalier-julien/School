#include "RenderDevice.h"

#include "SvcLog.h"
#include "SvcWindow.h"

#include <set>
#include <algorithm>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

RenderDevice* RenderDevice::ms_instance = nullptr;

bool RenderDevice::Create()
{
	ms_instance = new RenderDevice();

	return ms_instance->Init();
}

void RenderDevice::Destroy()
{
	ms_instance->Release();

	delete ms_instance;
	ms_instance = nullptr;
}

RenderDevice* RenderDevice::GetInstance()
{
	return ms_instance;
}

bool RenderDevice::CreateUniformBuffer(size_t bufferSize, Buffer* uniformBuffer)
{
	VkBuffer vkUniformBuffer;
	VkDeviceMemory vkUniformBufferMemory;
	if (!createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkUniformBuffer, vkUniformBufferMemory))
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create uniform buffer!");
		return false;
	}

	*uniformBuffer = new Buffer_T;
	(*uniformBuffer)->handle = vkUniformBuffer;
	(*uniformBuffer)->bufferMemory = vkUniformBufferMemory;

	return true;
}

bool RenderDevice::CreateIndexBuffer(const void* buffer, size_t bufferSize, Buffer* indexBuffer)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	if (! createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory))
		return false;

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, buffer, bufferSize);
	vkUnmapMemory(m_device, stagingBufferMemory);

	VkBuffer vkIndexBuffer;
	VkDeviceMemory vkIndexBufferMemory;
	if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkIndexBuffer, vkIndexBufferMemory))
	{
		vkDestroyBuffer(m_device, stagingBuffer, nullptr);
		vkFreeMemory(m_device, stagingBufferMemory, nullptr); 
		return false;
	}

	copyBuffer(stagingBuffer, vkIndexBuffer, bufferSize);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);

	*indexBuffer = new Buffer_T;
	(*indexBuffer)->handle = vkIndexBuffer;
	(*indexBuffer)->bufferMemory = vkIndexBufferMemory;

	return true;
}

void RenderDevice::BindIndexBuffer(CommandBuffer commandBuffer, Buffer buffer, size_t offset)
{
	vkCmdBindIndexBuffer(commandBuffer, buffer->handle, offset, VK_INDEX_TYPE_UINT16);
}

bool RenderDevice::CreateStorageBuffer(size_t bufferSize, Buffer* storageBuffer)
{
	VkBuffer vkStorageBuffer;
	VkDeviceMemory vkStorageBufferMemory;
	if (!createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkStorageBuffer, vkStorageBufferMemory))
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create storage buffer!");
		return false;
	}

	*storageBuffer = new Buffer_T;
	(*storageBuffer)->handle = vkStorageBuffer;
	(*storageBuffer)->bufferMemory = vkStorageBufferMemory;

	return true;
}

bool RenderDevice::CreateTexture(size_t width, size_t height, Format format, ImageTiling tiling, ImageUsageFlags usage, MemoryPropertyFlags properties, Texture* texture)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(width);
	imageInfo.extent.height = static_cast<uint32_t>(height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkImage image;
	if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create image!");
		return false;
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	VkDeviceMemory imageMemory;
	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to allocate image memory!");
		vkDestroyImage(m_device, image, nullptr);
		return false;
	}

	vkBindImageMemory(m_device, image, imageMemory, 0);

	VkImageView imageView;
	if (!CreateImageView(image, VK_FORMAT_R8G8B8A8_UNORM, &imageView))
	{
		vkDestroyImage(m_device, image, nullptr);
		vkFreeMemory(m_device, imageMemory, nullptr);
		return false;
	}

	(*texture) = new Texture_T;
	(*texture)->handle = image;
	(*texture)->imageMemory = imageMemory;
	(*texture)->view = imageView;

	return true;
}

bool RenderDevice::CreateTexture(const void* pixels, size_t width, size_t height, Texture* texture)
{
	VkDeviceSize imageSize = static_cast<uint64_t>(width * height * 4);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(m_device, stagingBufferMemory);

	if (!CreateTexture(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture))
		return false;

	transitionImageLayout((*texture)->handle, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, (*texture)->handle, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	transitionImageLayout((*texture)->handle, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);

	return true;
}

void RenderDevice::DestroyTexture(Texture texture)
{
	if (texture)
	{
		DestroyImageView(texture->view);
		vkDestroyImage(m_device, texture->handle, nullptr);
		vkFreeMemory(m_device, texture->imageMemory, nullptr);
		delete texture;
	}
}

RenderDevice::Sampler RenderDevice::GetSampler(SamplerType samplerType)
{
	return m_samplers[samplerType];
}

bool RenderDevice::MapBuffer(Buffer buffer, size_t offset, size_t size, void** data)
{
	return vkMapMemory(m_device, buffer->bufferMemory, offset, size, 0, data) == VK_SUCCESS;
}

void RenderDevice::UnmapBuffer(Buffer buffer)
{
	vkUnmapMemory(m_device, buffer->bufferMemory);
}

void RenderDevice::DestroyBuffer(Buffer buffer)
{
	if (buffer)
	{
		vkDestroyBuffer(m_device, buffer->handle, nullptr);
		vkFreeMemory(m_device, buffer->bufferMemory, nullptr);
		delete buffer;
	}
}

bool RenderDevice::CreateImageView(Image& image, Format format, ImageView* imageView)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(m_device, &createInfo, nullptr, imageView) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create swap chain image view !");
		return false;
	}

	return true;
}

void RenderDevice::DestroyImageView(ImageView imageView)
{
	vkDestroyImageView(m_device, imageView, nullptr);
}


bool RenderDevice::CreateSwapchain(Swapchain* swapchain)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
	uint32_t queuefamilyIndices[] =
	{
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queuefamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR swapchainKHR;
	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &swapchainKHR) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create swap cahin!");
		return false;
	}

	*swapchain = new Swapchain_T;
	(*swapchain)->handle = swapchainKHR;
	(*swapchain)->format = surfaceFormat.format;
	(*swapchain)->extent = extent;

	vkGetSwapchainImagesKHR(m_device, swapchainKHR, &imageCount, nullptr);
	(*swapchain)->images.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, swapchainKHR, &imageCount, (*swapchain)->images.data());

	return true;
}

void RenderDevice::DestroySwapchain(Swapchain swapchain)
{
	if (swapchain)
	{
		vkDestroySwapchainKHR(m_device, swapchain->handle, nullptr);
		delete swapchain;
	}
}

RenderDevice::Result RenderDevice::AcquireNextImage(Swapchain swapchain, u64 timeout, Semaphore semaphore, Fence fence, uint32_t* imageIndex)
{
	return vkAcquireNextImageKHR(m_device, swapchain->handle, timeout, semaphore, fence, imageIndex);
}

bool RenderDevice::QueueSubmit(size_t commandBufferCount, CommandBuffer* commandBuffers, Semaphore waitSemaphore, Semaphore signalSemaphore, Fence fence)
{
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { waitSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = static_cast<uint32_t>(commandBufferCount);
	submitInfo.pCommandBuffers = commandBuffers;

	VkSemaphore signalSemaphores[] = { signalSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to submit draw command buffer!");
		return false;
	}

	return true;
}

RenderDevice::Result RenderDevice::QueuePresent(Swapchain swapchain, Semaphore semaphore, uint32_t imageIndex)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	VkSemaphore signalSemaphores[] = { semaphore };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchain->handle };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	return vkQueuePresentKHR(m_presentQueue, &presentInfo);
}

bool RenderDevice::CreateRenderPass(Format format, RenderPass* renderPass)
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, renderPass) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create render pass!");
		return false;
	}

	return true;
}

void RenderDevice::DestroyRenderPass(RenderPass renderPass)
{
	vkDestroyRenderPass(m_device, renderPass, nullptr);
}

void RenderDevice::BeginRenderPass(CommandBuffer commandBuffer, RenderPass renderPass, Framebuffer framebuffer, size_t width, size_t height)
{
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void RenderDevice::EndRenderPass(CommandBuffer commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer);
}

bool RenderDevice::CreateFramebuffer(ImageView imageView, RenderPass renderPass, uint32_t width, uint32_t height, Framebuffer* framebuffer)
{
	VkImageView  attachments[] = { imageView };

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, framebuffer) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create framebuffer !");
		return false;
	}

	return true;
}

void RenderDevice::DestroyFramebuffer(Framebuffer framebuffer)
{
	vkDestroyFramebuffer(m_device, framebuffer, nullptr);
}

bool RenderDevice::CreateGraphicsPipeline(const char* shaderName, RenderPass renderPass, size_t width, size_t height, size_t setLayoutCount, DescriptorSetLayout* setLayouts, Pipeline* pipeline)
{
	std::string shaderFileName = "../../data/shaders/";
	shaderFileName += shaderName;

	std::string vShaderName = shaderFileName;
	vShaderName += ".vso";

	std::vector<char> vertShaderCode;
	if (!readFile(vShaderName.c_str(), vertShaderCode))
		return false;

	std::string fShaderName = shaderFileName;
	fShaderName += ".fso";

	std::vector<char> fragShaderCode;
	if (!readFile(fShaderName.c_str(), fragShaderCode))
		return false;

	VkShaderModule vertShaderModule;
	if (!createShaderModule(vertShaderCode, vertShaderModule))
		return false;

	VkShaderModule fragShaderModule;
	if (!createShaderModule(fragShaderCode, fragShaderModule))
		return false;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] =
	{
		vertShaderStageInfo,
		fragShaderStageInfo
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)width;
	viewport.height = (float)height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent.width = (uint32_t)width;
	scissor.extent.height = (uint32_t)height;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayoutCount);
	pipelineLayoutInfo.pSetLayouts = setLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	VkPipelineLayout pipelineLayout;
	if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create piepline layout!");
		return false;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VkPipeline graphicsPipeline;
	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create graphics pipeline!");
		return false;
	}

	vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_device, vertShaderModule, nullptr);

	*pipeline = new Pipeline_T;
	(*pipeline)->handle = graphicsPipeline;
	(*pipeline)->layout = pipelineLayout;

	return true;
}

void RenderDevice::DestroyGraphicsPipeline(Pipeline pipeline)
{
	if (pipeline)
	{
		vkDestroyPipeline(m_device, pipeline->handle, nullptr);
		vkDestroyPipelineLayout(m_device, pipeline->layout, nullptr);
		delete pipeline;
	}
}
void RenderDevice::BindGrapchicsPipeline(CommandBuffer commandBuffer, Pipeline pipeline)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);
}

bool RenderDevice::CreateDescriptorSetLayout(size_t descriptorCount, const DescriptorType* descriptorTypes, ShaderStageFlags stageFlags, DescriptorSetLayout* descriptorSetLayout)
{
	VkDescriptorSetLayoutBinding binding;
	binding.descriptorCount = 1;
	binding.stageFlags = stageFlags;
	binding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> bindings(descriptorCount, binding);
	for (size_t i = 0; i < descriptorCount; ++i)
	{
		bindings[i].binding = static_cast<uint32_t>(i);
		bindings[i].descriptorType = descriptorTypes[i];
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(descriptorCount);
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, descriptorSetLayout) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create descriptor set layout!");
		return false;
	}

	return true;
}

void RenderDevice::DestroyDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout)
{
	vkDestroyDescriptorSetLayout(m_device, descriptorSetLayout, nullptr);
}

bool RenderDevice::CreateDescriptorPool(size_t poolCount, const DescriptorType* descriptorTypes, size_t descriptorCount, DescriptorPool* descriptorPool)
{
	std::vector<VkDescriptorPoolSize> poolSize(poolCount);
	for (size_t i = 0; i < poolCount; ++i)
	{
		poolSize[i].type = descriptorTypes[i];
		poolSize[i].descriptorCount = static_cast<uint32_t>(descriptorCount);
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolCount);
	poolInfo.pPoolSizes = poolSize.data();
	poolInfo.maxSets = static_cast<uint32_t>(descriptorCount);

	if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, descriptorPool) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create descriptor pool!");
		return false;
	}

	return true;
}

void RenderDevice::DestroyDescriptorPool(DescriptorPool descriptorPool)
{
	vkDestroyDescriptorPool(m_device, descriptorPool, nullptr);
}

bool RenderDevice::AllocateDescriptorSets(DescriptorPool descriptorPool, size_t descriptorSetCount, DescriptorSetLayout* layouts, DescriptorSet* descriptorSets)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetCount);
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(m_device, &allocInfo, descriptorSets) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to allocate descriptor sets!");
		return false;
	}

	return true;
}
void RenderDevice::FreeDescriptorSets(DescriptorPool descriptorPool, size_t descriptorSetCount, DescriptorSet* descriptorSets)
{
	vkFreeDescriptorSets(m_device, descriptorPool, static_cast<uint32_t>(descriptorSetCount), descriptorSets);
}

void RenderDevice::UpdateDescriptorSet(DescriptorSet descriptorSet, size_t bufferCount, const DescriptorBufferInfo* buffers, size_t imageCount, const DescriptorImageInfo* images)
{
	VkWriteDescriptorSet blank = {};
	std::vector< VkWriteDescriptorSet> descriptorWrite(bufferCount + imageCount, blank);
	uint32_t binding = 0;
	
	std::vector<VkDescriptorBufferInfo> bufferInfo(bufferCount);
	for (size_t i = 0; i < bufferCount; ++i, ++binding)
	{
		bufferInfo[i].buffer = buffers[i].buffer->handle;
		bufferInfo[i].offset = buffers[i].offset;
		bufferInfo[i].range = buffers[i].range;

		descriptorWrite[binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[binding].dstSet = descriptorSet;
		descriptorWrite[binding].dstBinding = binding;
		descriptorWrite[binding].dstArrayElement = 0;
		descriptorWrite[binding].descriptorType = buffers[i].type;
		descriptorWrite[binding].descriptorCount = 1;
		descriptorWrite[binding].pBufferInfo = &bufferInfo[i];
	}

	std::vector<VkDescriptorImageInfo> imageInfo(imageCount);
	for (size_t i = 0; i < imageCount; ++i, ++binding)
	{
		imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[i].imageView = images[i].texture->view;
		imageInfo[i].sampler = images[i].sampler;

		descriptorWrite[binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[binding].dstSet = descriptorSet;
		descriptorWrite[binding].dstBinding = binding;
		descriptorWrite[binding].dstArrayElement = 0;
		descriptorWrite[binding].descriptorType = images[i].type;
		descriptorWrite[binding].descriptorCount = 1;
		descriptorWrite[binding].pImageInfo = &imageInfo[i];
	}

	vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptorWrite.size()), descriptorWrite.data(), 0, nullptr);
}

void RenderDevice::BindDescriptorSets(CommandBuffer commandBuffer, Pipeline pipeline, size_t firstSet, size_t descriptorSetCount, DescriptorSet* descriptorSets)
{
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->layout, static_cast<uint32_t>(firstSet), static_cast<uint32_t>(descriptorSetCount), descriptorSets, 0, nullptr);
}

bool RenderDevice::AllocateCommandBuffers(size_t count, CommandBuffer* commandBuffers)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)count;

	if (vkAllocateCommandBuffers(m_device, &allocInfo, commandBuffers) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to allocate command buffers!");
		return false;
	}

	return true;
}

void RenderDevice::FreeCommandBuffers(size_t count, CommandBuffer* commandBuffers)
{
	vkFreeCommandBuffers(m_device, m_commandPool, (uint32_t)count, commandBuffers);
}

bool RenderDevice::BeginCommandBuffer(CommandBuffer commandBuffer)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to begin recording command buffer !");
		return false;
	}

	return true;
}
bool RenderDevice::EndCommandBuffer(CommandBuffer commandBuffer)
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to record command buffer !");
		return false;
	}

	return true;
}

bool RenderDevice::CreateSemaphore(Semaphore* semaphore)
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, semaphore) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create semaphore !");
		return false;
	}

	return true;
}

void RenderDevice::DestroySemaphore(Semaphore semaphore)
{
	vkDestroySemaphore(m_device, semaphore, nullptr);
}

bool RenderDevice::CreateFence(Fence* fence)
{
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (	vkCreateFence(m_device, &fenceInfo, nullptr, fence) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create fence !");
		return false;
	}

	return true;
}

void RenderDevice::DestroyFence(Fence fence)
{
	vkDestroyFence(m_device, fence, nullptr);
}

void RenderDevice::Draw(CommandBuffer commandBuffer, u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
{
	vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void RenderDevice::DrawIndexed(CommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance)
{
	vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void RenderDevice::DeviceWaitIdle()
{
	vkDeviceWaitIdle(m_device);
}

void RenderDevice::WaitForFences(size_t count, const Fence* fences, bool waitAll, u64 timeout)
{
	vkWaitForFences(m_device, static_cast<uint32_t>(count), fences, waitAll, timeout);
}

void RenderDevice::ResetFences(size_t count, const Fence* fences)
{
	vkResetFences(m_device, static_cast<uint32_t>(count), fences);
}

RenderDevice::RenderDevice()
	: m_instance(nullptr)
	, m_debugMessenger(nullptr)
	, m_surface(nullptr)
	, m_physicalDevice(nullptr)
	, m_device(nullptr)
	, m_graphicsQueue(nullptr)
	, m_presentQueue(nullptr)
	, m_enableValidationLayers(false)
{
}

bool RenderDevice::Init()
{
	m_validationLayers.push_back("VK_LAYER_KHRONOS_validation");

	m_deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	m_enableValidationLayers = true;

	if (!createInstance())
		return false;

	if (!setupDebugMessenger())
		return false;

	if (!createSurface())
		return false;

	if (!pickPhysicalDevice())
		return false;

	if (!createLogicalDevice())
		return false;

	if (!createCommandPool())
		return false;

	if (!createSamplers())
		return false;

	return true;
}

void RenderDevice::Release()
{
	vkDeviceWaitIdle(m_device);

	for (VkSampler& sampler : m_samplers)
	{
		vkDestroySampler(m_device, sampler, nullptr);
		sampler = nullptr;
	}

	vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	m_commandPool = nullptr;

	m_presentQueue = nullptr;
	m_graphicsQueue = nullptr;

	vkDestroyDevice(m_device, nullptr);
	m_device = nullptr;

	m_physicalDevice = VK_NULL_HANDLE;

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	m_surface = nullptr;

	if (m_enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
		m_debugMessenger = nullptr;
	}

	vkDestroyInstance(m_instance, nullptr);
	m_instance = nullptr;
}

bool RenderDevice::createInstance()
{
	if (m_enableValidationLayers && !checkValidationLayerSupport()) {
		SvcLog::Printf(SvcLog::ELevel_Error, "validation layers requested, but not available!");
		return false;
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Test_02";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "School Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (m_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create instance!");
		return false;
	}

	return true;
}

void RenderDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	createInfo.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

bool RenderDevice::setupDebugMessenger()
{
	if (!m_enableValidationLayers)
		return true;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to set up debug messenger!");
		return false;
	}

	return true;
}

bool RenderDevice::createSurface()
{
	if (SvcWindow::GetInstance()->CreateVulkanWindowSurface(m_instance, nullptr, &m_surface) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create window surface!");
		return false;
	}

	return true;
}

bool RenderDevice::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to find GPUs with Vulkan support!");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	for (const VkPhysicalDevice& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to find a suitable GPU!");
		return false;
	}

	return true;
}

bool RenderDevice::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies =
	{
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

	if (m_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
	}

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create logical device!");
		return false;
	}

	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);

	return true;
}

bool RenderDevice::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create command pool!");
		return false;
	}

	return true;
}

bool RenderDevice::createSamplers()
{
	const VkFilter filters[] = {
		VK_FILTER_NEAREST,
		VK_FILTER_LINEAR
	};

	const VkSamplerMipmapMode mipFilters[] = {
		VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VK_SAMPLER_MIPMAP_MODE_LINEAR
	};

	const VkSamplerAddressMode addressModes[] = {
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VK_SAMPLER_ADDRESS_MODE_REPEAT
	};

	for (VkSampler& sampler : m_samplers)
	{
		sampler = nullptr;
	}

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	size_t samplerIndex = 0;
	for (size_t addressModeIndex = 0; addressModeIndex < 2; ++addressModeIndex)
	{
		samplerInfo.addressModeU = addressModes[addressModeIndex];
		samplerInfo.addressModeV = addressModes[addressModeIndex];
		samplerInfo.addressModeW = addressModes[addressModeIndex];

		for (size_t filterIndex = 0; filterIndex < 2; ++filterIndex)
		{
			samplerInfo.magFilter = filters[filterIndex];
			samplerInfo.minFilter = filters[filterIndex];
			samplerInfo.mipmapMode = mipFilters[filterIndex];

			if (vkCreateSampler(m_device, &samplerInfo, nullptr, m_samplers + samplerIndex++) != VK_SUCCESS)
			{
				SvcLog::Printf(SvcLog::ELevel_Error, "failed to crea texture sampler!");
				return false;
			}
		}
	}

	return true;
}

bool RenderDevice::createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create shaderModule!");
		return false;
	}

	return true;
}

VkSurfaceFormatKHR RenderDevice::chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR RenderDevice::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const VkPresentModeKHR& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D RenderDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		SvcWindow::GetInstance()->GetWindowSize(width, height);
		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = std::max(capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

RenderDevice::SwapChainSupportDetails RenderDevice::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	details.formats.resize(formatCount);
	if (formatCount != 0)
	{
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

	details.presentModes.resize(presentModeCount);
	if (presentModeCount != 0)
	{
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool RenderDevice::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.isComplete();
}

RenderDevice::QueueFamilyIndices RenderDevice::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
			break;

		++i;
	}

	return indices;
}

std::vector<const char*> RenderDevice::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}
bool RenderDevice::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_validationLayers)
	{
		bool layerFound = false;

		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}

bool RenderDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to create buffer!");
		return false;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		vkDestroyBuffer(m_device, buffer, nullptr);

		SvcLog::Printf(SvcLog::ELevel_Error, "failed to allocate buffer memory!");
		return false;
	}

	vkBindBufferMemory(m_device, buffer, bufferMemory, 0);

	return true;
}

VkCommandBuffer RenderDevice::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void RenderDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

void RenderDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

uint32_t RenderDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	return -1;
}

void RenderDevice::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer);
}

void RenderDevice::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
}


bool RenderDevice::readFile(const char* filename, std::vector<char>& buffer)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		SvcLog::Printf(SvcLog::ELevel_Error, "failed to open file %s!", filename);
		return false;
	}

	size_t fileSize = (size_t)file.tellg();
	buffer.resize(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return true;
}

VkBool32 RenderDevice::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	SvcLog::Printf(SvcLog::ELevel_Error, "validation layer: %s", pCallbackData->pMessage);

	return VK_FALSE;
}

