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
	typedef VkImageTiling ImageTiling;
	typedef VkImageUsageFlags ImageUsageFlags;
	typedef VkImageView ImageView;
	typedef VkSampler Sampler;
	typedef VkDescriptorType DescriptorType;
	typedef VkShaderStageFlags ShaderStageFlags;
	typedef VkMemoryPropertyFlags MemoryPropertyFlags;

	struct Buffer_T
	{
		VkBuffer			handle;
		VkDeviceMemory	bufferMemory;
	};
	typedef Buffer_T* Buffer;

	struct Texture_T
	{
		VkImage			handle;
		VkDeviceMemory	imageMemory;
		VkImageView		view;
	};
	typedef Texture_T* Texture;

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

	typedef VkDescriptorSetLayout DescriptorSetLayout;
	typedef VkDescriptorPool DescriptorPool;
	typedef VkDescriptorSet DescriptorSet;

	struct DescriptorBufferInfo
	{
		Buffer buffer;
		uint32_t offset;
		uint32_t range;
		DescriptorType type;
	};

	struct DescriptorImageInfo
	{
		Texture texture;
		Sampler sampler;
		DescriptorType type;
	};

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

	enum SamplerType
	{
		SamplerType_Nearest_Clamp,
		SamplerType_Linear_Clamp,
		SamplerType_Nearest_Repeat,
		SamplerType_Linear_Repeat,
		SamplerType_Count
	};

public:
	static bool Create();
	static void Destroy();

	static RenderDevice* GetInstance();

	bool CreateUniformBuffer(size_t bufferSize, Buffer* uniformBuffer);
	bool CreateIndexBuffer(const void* data, size_t size, Buffer* indexBuffer);
	void BindIndexBuffer(CommandBuffer commandBuffer, Buffer buffer, size_t offset);
	
	bool CreateStorageBuffer(size_t bufferSize, Buffer* storageBuffer);

	bool CreateTexture(size_t width, size_t height, Format format, ImageTiling tiling, ImageUsageFlags usage, MemoryPropertyFlags properties, Texture* texture);
	bool CreateTexture(const void* data, size_t width, size_t height, Texture* texture);
	void DestroyTexture(Texture texture);

	Sampler GetSampler(SamplerType samplerType);

	bool MapBuffer(Buffer buffer, size_t offset, size_t size, void** data);
	void UnmapBuffer(Buffer buffer);
	void DestroyBuffer(Buffer buffer);

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

	bool CreateGraphicsPipeline(const char* shaderName, RenderPass renderPass, size_t width, size_t height, size_t setLayoutCount, DescriptorSetLayout* setLayouts, Pipeline* pipeline);
	void DestroyGraphicsPipeline(Pipeline pipeline);
	void BindGrapchicsPipeline(CommandBuffer commandBuffer, Pipeline pipeline);

	bool CreateDescriptorSetLayout(size_t descriptorCount, const DescriptorType* descriptorTypes, ShaderStageFlags stageFlags, DescriptorSetLayout* descriptorSetLayout);
	void DestroyDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout);

	bool CreateDescriptorPool(size_t poolCount, const DescriptorType* descriptorTypes, size_t descriptorCount, DescriptorPool* descriptorPool);
	void DestroyDescriptorPool(DescriptorPool descriptorPool);

	bool AllocateDescriptorSets(DescriptorPool descriptorPool, size_t descriptorSetCount, const DescriptorSetLayout* layouts, DescriptorSet* descriptorSets);
	void FreeDescriptorSets(DescriptorPool descriptorPool, size_t descriptorSetCount, DescriptorSet* descriptorSets);
	void UpdateDescriptorSet(DescriptorSet descriptorSet, size_t bufferCount, const DescriptorBufferInfo* buffers, size_t imageCount, const DescriptorImageInfo* images);
	void BindDescriptorSets(CommandBuffer commandBuffer, Pipeline pipeline, size_t firstSet, size_t descriptorSetCount, const DescriptorSet* descriptorSets);

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
	Result GetFenceStatus(Fence fence);

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
	bool createSamplers();

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
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

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

	VkCommandPool m_commandPool;

	Sampler m_samplers[SamplerType_Count];

	bool m_enableValidationLayers;
	std::vector<const char*> m_validationLayers;
	std::vector<const char*> m_deviceExtensions;
};
