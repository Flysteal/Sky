#pragma once

#include <set>
#include "DebugUtils.h"
#include <optional>
#include <vector>
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace Visuals
{
    namespace PhysicalDevice
    {
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool IsComplete() const
            {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

        const std::vector<const char*> deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };


        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface)
        {
            QueueFamilyIndices indices;

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

            for (uint32_t i = 0; i < queueFamilyCount; ++i)
            {
                const auto& qFamily = queueFamilies[i];

                // Graphics support
                if (qFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    indices.graphicsFamily = i;
                }

                // Presentation support (uncomment when needed)
                VkBool32 presentSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
                if (presentSupport)
                {
                    indices.presentFamily = i;
                }

                if (indices.IsComplete())
                {
                    break;
                }
            }

            return indices;
        }

        bool CheckDeviceExtensionSupport(VkPhysicalDevice& physicalDevice)
        {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

            std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto& extension : availableExtensions)
            {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }

        bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR& surface)
        {
            QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
            bool extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

            bool swapChainAdequate = false;

            if (extensionsSupported)
            {
                // Query surface capabilities
                VkSurfaceCapabilitiesKHR capabilities;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

                // Query available surface formats
                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

                if (0 == formatCount)
                {
                    swapChainAdequate = false; // No formats available
                }

                std::vector<VkSurfaceFormatKHR> formats(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

                // Query supported presentation modes
                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

                if (0 == presentModeCount)
                {
                    swapChainAdequate = false;
                }

                std::vector<VkPresentModeKHR> presentModes(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

                // Return true if both formats and present modes are available
                swapChainAdequate = true;
            }

            // Further checks for extensions, swap chain support, etc.
            return indices.IsComplete() && extensionsSupported && swapChainAdequate;
        }

        void Pick(VkInstance& instance, VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface)
        {
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            for (const auto& device : devices)
            {
                if (IsDeviceSuitable(device, surface))
                {
                    physicalDevice = device;
                    break;
                }
            }

            if (VK_NULL_HANDLE == physicalDevice)
            {
                throw std::runtime_error("Failed to find a suitable GPU!");
            }
        }
    }

    namespace LogicalDevice
    {

        void Create(VkPhysicalDevice& physicalDevice, VkDevice& device, const std::vector<const char*> validationLayers, VkQueue& graphicsQueue, VkQueue& presentQueue, VkSurfaceKHR& surface)
        {
            PhysicalDevice::QueueFamilyIndices indices = PhysicalDevice::FindQueueFamilies(physicalDevice, surface);

            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
            std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

            float queuePriority = 1.0f;
            for (uint32_t queueFamily : uniqueQueueFamilies)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount       = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.push_back(queueCreateInfo);
            }

            VkPhysicalDeviceFeatures deviceFeatures{};

            VkDeviceCreateInfo createInfo{};
            createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
            createInfo.pQueueCreateInfos       = queueCreateInfos.data();
            createInfo.pEnabledFeatures        = &deviceFeatures;
            createInfo.enabledExtensionCount   = static_cast<uint32_t>(PhysicalDevice::deviceExtensions.size());
            createInfo.ppEnabledExtensionNames = PhysicalDevice::deviceExtensions.data();

            if (true == kDebug)
            {
                createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();
            }
            else
            {
                createInfo.enabledLayerCount   = 0;
            }

            if (VK_SUCCESS != vkCreateDevice(physicalDevice, &createInfo, nullptr, &device))
            {
                throw std::runtime_error("Failed to create logical device !");
            }

            vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
            vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
        }

        void Destroy(VkDevice& device)
        {
            vkDestroyDevice(device, nullptr);
        }
    }
}