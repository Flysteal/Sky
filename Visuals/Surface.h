#pragma once

#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Visuals
{
    namespace Surface
    {
        void Create(GLFWwindow* window, VkInstance& instance, VkSurfaceKHR& surface)
        {
            if (instance == VK_NULL_HANDLE)
            {
                throw std::runtime_error("Invalid Vulkan instance!");
            }

            if (VK_SUCCESS != glfwCreateWindowSurface(instance, window, nullptr, &surface))
            {
                throw std::runtime_error("Failed to create window surface !");
            }
        }

        void Destroy(VkInstance& instance, VkSurfaceKHR& surface)
        {
            if (surface != VK_NULL_HANDLE)
            {
                vkDestroySurfaceKHR(instance, surface, nullptr);
                surface = VK_NULL_HANDLE;
            }
        }
    }
}