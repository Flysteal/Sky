#pragma once

#include "DebugUtils.h"
#include <optional>
#include <vector>

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

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& physicalDevice)
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

                // Presentation support – query per‑family
                // VkBool32 presentSupport = VK_FALSE;
                // vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_surface, &presentSupport);
                // if (presentSupport)
                // {
                //     indices.presentFamily = i;
                // }

                if (indices.IsComplete())
                {
                    break;
                }
            }

            return indices;
        }

        bool IsDeviceSuitable(VkPhysicalDevice physicalDevice)
        {
            QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
            // bool extensionsSupported = CheckDeviceExtensionSupport(device);

            // return indices.IsComplete() && extensionsSupported;
            return indices.IsComplete();
        }

        void Pick(VkInstance instance, VkPhysicalDevice physicalDevice)
        {
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            for (const auto& device : devices)
            {
                if (true == IsDeviceSuitable(device))
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
        void Create(VkPhysicalDevice& physicalDevice, VkDevice& device, const std::vector<const char*> validationLayers, VkQueue graphicsQueue)
        {
            PhysicalDevice::QueueFamilyIndices indices = PhysicalDevice::FindQueueFamilies(physicalDevice);

            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
            queueCreateInfo.queueCount = 1;
            float queuePriority = 1.0f;
            queueCreateInfo.pQueuePriorities = &queuePriority;


            VkPhysicalDeviceFeatures deviceFeatures{};

            VkDeviceCreateInfo createInfo{};
            createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos    = &queueCreateInfo;
            createInfo.queueCreateInfoCount = 1;
            createInfo.pEnabledFeatures     = &deviceFeatures;
            createInfo.enabledExtensionCount = 0;

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
                throw std::runtime_error("failed to create logical device!");
            }

            vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        }
    }
}