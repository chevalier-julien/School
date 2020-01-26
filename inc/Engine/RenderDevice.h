#pragma once

#include <vulkan/vulkan.h>

#include "Types.h"

#include <vector>
#include <optional>
#include <map>
#include <string>

class RenderDevice
{
public:
	typedef VkResult Result;
	typedef VkFormat Format;
	typedef VkImage Image;
	typedef VkImageView ImageView;

	struct Buffer_T
	{
		VkBuffer			handle;
		VkDeviceMemory	indexBufferMemory;
	};
	typedef Buffer_T* Buffer;

	struct Swapchain_T
	{
		VkSwapchainKHR				handle;
		VkExtent2D					extent;
		Format						format;
		std::vector<Image>			images;
	};
	typedef Swapchain_T* Swapchain;

	typedef VkRenderPass RenderPass;
	typedef VkFramebuffer Framebuffer;

	struct Pipeline_T
	{
		VkPipeline			handle;
		VkPipelineLayout		layout;
	};
	typedef Pipeline_T* Pipeline;

	typedef VkCommandBuffer CommandBuffer;
	typedef VkSemaphore Semaphore;
	typedef VkFence Fence;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value()
				&& presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR			capabilities;
		std::vector<VkSurfaceFormatKHR>	formats;
		std::vector<VkPresentModeKHR>	presentModes;
	};

public:
	static bool Create();
	static void Destroy();

	static RenderDevice* GetInstance();

	bool CreateIndexBuffer(void* data, size_t size, Buffer* indexBuffer);
	void DestroyIndexBuffer(Buffer buffer);
	void BindIndexBuffer(CommandBuffer commandBuffer, Buffer buffer, size_t offset);

	bool CreateImageView(Image& image, Format format, ImageView* imageView);
	void DestroyImageView(ImageView imageView);

	bool CreateSwapchain(Swapchain* swapchain);
	void DestroySwapchain(Swapchain swapchain);
	Result AcquireNextImage(Swapchain swapchain, u64 timeout, Semaphore semaphore, Fence fence, uint32_t* imageIndex);
	bool QueueSubmit(size_t commandBufferCount, CommandBuffer* commandBuffers, Semaphore waitSemaphore, Semaphore signalSemaphore, Fence fence);
	Result QueuePresent(Swapchain swapchain, Semaphore semaphore, uint32_t imageIndex);

	bool CreateRenderPass(Format format, RenderPass* renderPass);
	void DestroyRenderPass(RenderPass renderPass);
	void BeginRenderPass(CommandBuffer commandBuffer, RenderPass renderPass, Framebuffer framebuffer, size_t width, size_t height);
	void EndRenderPass(CommandBuffer commandBuffer);

	bool CreateFramebuffer(ImageView imageView, RenderPass renderPass, uint32_t width, uint32_t height, Framebuffer* framebuffer);
	void DestroyFramebuffer(Framebuffer framebuffer);

	bool CreateGraphicsPipeline(const char* shaderName, RenderPass renderPass, size_t width, size_t height, Pipeline* pipeline);
	void DestroyGraphicsPipeline(Pipeline pipeline);
	void BindGrapchicsPipeline(CommandBuffer commandBuffer, Pipeline pipeline);

	bool AllocateCommandBuffers(size_t count, CommandBuffer* commandBuffers);
	void FreeCommandBuffers(size_t count, CommandBuffer* commandBuffers);
	bool BeginCommandBuffer(CommandBuffer commandBuffer);
	bool EndCommandBuffer(CommandBuffer commandBuffer);

	bool CreateSemaphore(Semaphore* semaphore);
	void DestroySemaphore(Semaphore semaphore);

	bool CreateFence(Fence* fence);
	void DestroyFence(Fence fence);

	void Draw(CommandBuffer commandBuffer, u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 fistInstance);
	void DrawIndexed(CommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance);

	void DeviceWaitIdle();
	void WaitForFences(size_t count, const Fence* fences, bool waitAll, u64 timeout);
	void ResetFences(size_t count, const Fence* fences);

private:
	RenderDevice();
	~RenderDevice() {}

	bool Init();
	void Release();

	bool createInstance();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool setupDebugMessenger();
	bool createSurface();
	bool pickPhysicalDevice();
	bool createLogicalDevice();
	bool createCommandPool();

	bool createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule);

	VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentMode);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();

	bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	static bool readFile(const char* filename, std::vector<char>& buffer);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

private:
	static RenderDevice* ms_instance;

	VkInstance	m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;

	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	/*
	VkDescriptorSetLayout m_descriptorSetLayout;*/
	std::map<std::string, Pipeline> m_graphicsPipelines;

	VkCommandPool m_commandPool;

	bool m_enableValidationLayers;
	std::vector<const char*> m_validationLayers;
	std::vector<const char*> m_deviceExtensions;
};
