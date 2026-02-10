#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "ValidationLayers.h"

class Vulkan
{
public:
    Vulkan()
        :m_instance{VK_NULL_HANDLE}
    {}

    void Init()
    {
        CreateInstance();
    }

    VkInstance Get() const 
    {
        return m_instance;
    }

    void Destroy()
    {
        if (m_instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }
    }

private:
    VkInstance m_instance;

    std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t count = 0;
        const char** glfwExt = glfwGetRequiredInstanceExtensions(&count);
        std::vector<const char*> extensions(glfwExt, glfwExt + count);

        // Debug utils needed for validation layers
        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // Portability extension required on macOS, etc.
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        return extensions;
    }

    void CreateInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "SkyLands";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "Sky";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            ValidationLayers::PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount   = 0;
        }

        // Use the unified helper
        auto extensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Set the portability flag only when the extension is present
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }

        // Optional debug output
        if (enableValidationLayers)
        {
            uint32_t extCount = 0;
            VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
            if (res != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to query extension count");
            }

            std::vector<VkExtensionProperties> exts(extCount);
            res = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, exts.data());
            if (res != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to enumerate extensions");
            }

            std::cout << "Available instance extensions:\n";
            for (const auto& e : exts)
            {
                std::cout << "\t" << e.extensionName << '\n';
            }
        }
    }


};