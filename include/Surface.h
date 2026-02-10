#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Surface
{
public:
    Surface()
        :m_surface{VK_NULL_HANDLE}, m_instance(VK_NULL_HANDLE), m_window(nullptr)
    {}

    void Init(VkInstance instance, GLFWwindow* window)
    {
        m_instance = instance;
        m_window = window;
        CreateSurface();
    }

    void CreateSurface()
    {
        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    VkSurfaceKHR Get() const
    {
        return m_surface;
    }


    void Destroy()
    {
        if (m_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }
    }


private:
    VkSurfaceKHR m_surface;
    VkInstance m_instance;
    GLFWwindow* m_window;



};