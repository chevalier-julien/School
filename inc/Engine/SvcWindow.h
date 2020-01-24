#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class SvcWindow
{
public:
	static bool Create();
	static void Destroy();
	static bool Update();

	static SvcWindow* GetInstance();

	void GetWindowSize(int& width, int& height);
	void GetFramebufferSize(int& width, int& height);

	void WaitEvents();
	bool WindowShouldClose();

	VkResult CreateVulkanWindowSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);

	void SetKeyCallback(GLFWkeyfun cb);

private:
	SvcWindow() {}
	~SvcWindow() {}

	bool Init();
	void Release();

private:
	static SvcWindow* ms_instance;

	GLFWwindow*	m_window;
};
