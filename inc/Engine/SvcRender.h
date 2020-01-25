#pragma once

#include <vulkan/vulkan.h>

#include "Types.h"
#include "TileRenderer.h"

#include <vector>
#include <optional>
#include <map>
#include <string>

struct IndexBuffer
{
	VkBuffer			handle;
	VkDeviceMemory	indexBufferMemory;
};

struct RenderPipeline
{
	VkPipeline			handle;
	VkPipelineLayout		layout;
};

typedef VkCommandBuffer RenderCommandBuffer;

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


class SvcRender
{
public:
	static bool Create();
	static void Destroy();
	static bool Update();

	static SvcRender* GetInstance();

	IndexBuffer* CreateIndexBuffer(void* data, size_t size);
	void DestroyIndexBuffer(IndexBuffer* buffer);
	void BindIndexBuffer(IndexBuffer* buffer, size_t offset);

	bool CreateGraphicsPipeline(const char* shaderName);
	void DestroyGraphicsPipeline(const char* shaderName);
	void BindGrapchicsPipeline(const char* shaderName);

	void Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 fistInstance);
	void DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance);

private:
	SvcRender() {}
	~SvcRender() {}

	bool Init();
	void Release();

	bool createInstance();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool setupDebugMessenger();
	bool createSurface();
	bool pickPhysicalDevice();
	bool createLogicalDevice();
	bool createSwapChain();
	bool createImageViews();
	bool createRenderPass();
	bool createFramebuffers();
	bool createCommandPool();
	bool createCommandBuffers();
	bool createSynchObjects();

	bool drawFrame();

	bool createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule);

	void cleanupSwapChain();
	bool recreateSwapChain();
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
	static SvcRender* ms_instance;

	VkInstance	m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;

	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat	 m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;

	VkRenderPass m_renderPass;
	std::map<std::string,RenderPipeline> m_graphicsPipelines;

	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;

	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	std::vector<VkFence> m_imagesInFlight;
	size_t m_currentFrame;
	size_t m_commandBufferIndex;

	bool m_enableValidationLayers;
	std::vector<const char*> m_validationLayers;
	std::vector<const char*> m_deviceExtensions;
	size_t m_maxFramesInFlight;

	TileRenderer m_tileRenderer;
};
