#include "SvcWindow.h"

#include <glm/vec2.hpp>

SvcWindow* SvcWindow::ms_instance = nullptr;

bool SvcWindow::Create()
{
	ms_instance = new SvcWindow();

	return ms_instance->Init();
}

void SvcWindow::Destroy()
{
	ms_instance->Release();

	delete ms_instance;
	ms_instance = nullptr;
}

bool SvcWindow::Init()
{
	glm::uvec2 size(800, 600);
	const char* name = "My window";

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(size[0], size[1], name, nullptr, nullptr);

	return m_window != nullptr;
}

void SvcWindow::Release()
{
	glfwDestroyWindow(ms_instance->m_window);
	ms_instance->m_window = nullptr;

	glfwTerminate();
}

bool SvcWindow::Update()
{
	glfwPollEvents();

	return true;
}

void SvcWindow::GetWindowSize(int& width, int& height)
{
	glfwGetWindowSize(ms_instance->m_window, &width, &height);
}

void SvcWindow::GetFramebufferSize(int& width, int& height)
{
	glfwGetFramebufferSize(ms_instance->m_window, &width, &height);
}

void SvcWindow::WaitEvents()
{
	glfwWaitEvents();
}

bool SvcWindow::WindowShouldClose()
{
	return glfwWindowShouldClose(ms_instance->m_window);
}

VkResult SvcWindow::CreateVulkanWindowSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface)
{
	return glfwCreateWindowSurface(instance, ms_instance->m_window, allocator, surface);
}
