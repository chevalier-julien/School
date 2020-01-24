#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class SvcWindow
{
public:
	static bool Create();
	static void Destroy();
	static bool Update();

	static void GetWindowSize(int& width, int& height);
	static void GetFramebufferSize(int& width, int& height);

	static void WaitEvents();
	static bool WindowShouldClose();

	static VkResult CreateVulkanWindowSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);

private:
	SvcWindow() {}
	~SvcWindow() {}

	bool Init();
	void Release();

private:
	static SvcWindow* ms_instance;

	GLFWwindow*	m_window;
};
