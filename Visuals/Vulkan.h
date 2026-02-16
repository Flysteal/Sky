#pragma once
#include "Visuals.h"
#include "Window.h"
#include <cstddef>
#include <vulkan/vulkan_core.h>
#include <vector>
#include "DebugUtils.h"
#include "GraphicsPipeline.h"


namespace Visuals
{
    namespace Instance
    {
        std::vector<const char*> GetRequiredExtensions();

        void Create(VkInstance& instance, const char* appName, const std::vector<const char*> validationLayers)
        {
            VkApplicationInfo appInfo{};
            appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pNext              = nullptr;
            appInfo.pApplicationName   = appName;
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName        = "Sky";
            appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 3);
            appInfo.apiVersion         = VK_API_VERSION_1_3;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.flags                  |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            createInfo.pApplicationInfo        = &appInfo;


            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (true == kDebug)
            {
                createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                DebugUtils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
            }
            else 
            {
                createInfo.enabledLayerCount   = 0;
                createInfo.pNext               = nullptr;
            }


            std::vector<const char*> extensions = GetRequiredExtensions();
            createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();


            if (VK_SUCCESS != vkCreateInstance(&createInfo, nullptr, &instance))
            {
                throw std::runtime_error("Failed to create VULKAN instance !");
            }
        }

        void Destroy(VkInstance& instance)
        {
            vkDestroyInstance(instance, nullptr);
        }

        std::vector<const char*> GetRequiredExtensions()
        {
            uint32_t count = 0;
            const char** glfwExt = glfwGetRequiredInstanceExtensions(&count);
            std::vector<const char*> extensions(glfwExt, glfwExt + count);

            if (true == kDebug)
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            // required on MacOS.
            extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            return extensions;
        }
    }
}