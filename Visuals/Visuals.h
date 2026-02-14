#pragma once
#include "Window.h"
#include "Vulkan.h"
#include <cstddef>
#include <vulkan/vulkan_core.h>
#include "DebugUtils.h"
#include "Device.h"

namespace Visuals
{
    struct Visuals
    {
        Visuals()
            :m_window(nullptr),
            m_height(600), m_width(800),
            m_name("SkyLands"),
            m_instance{VK_NULL_HANDLE},
            m_debugMessenger{VK_NULL_HANDLE},
            m_physicalDevice{VK_NULL_HANDLE},
            m_device{VK_NULL_HANDLE},
            m_graphicsQueue{VK_NULL_HANDLE}
        {
            Create();
            Destroy();
        }

    private:
        GLFWwindow* m_window;
        uint32_t m_height;
        uint32_t m_width;
        const char* m_name;
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice m_physicalDevice;
        VkDevice m_device;
        VkQueue m_graphicsQueue;


        void Create()
        {
            Glfw::Create();
            Window::Create(m_window, m_width, m_height, m_name);
            DebugUtils::CheckSupport(DebugUtils::validationLayers);
            Instance::Create(m_instance, m_name, DebugUtils::validationLayers);
            DebugUtils::Create(m_instance, m_debugMessenger);
            PhysicalDevice::Pick(m_instance, m_physicalDevice);
            LogicalDevice::Create(m_physicalDevice, m_device, const std::vector<const char *> validationLayers, VkQueue graphicsQueue)
        }

        void Destroy()
        {
            DebugUtils::Destroy(m_instance, m_debugMessenger);
            Instance::Destroy(m_instance);
            Window::Destroy(m_window);
            Glfw::Destroy();
        }
    };
}