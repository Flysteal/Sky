#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <algorithm>
#include <set>

static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class Device
{
public:
    Device()
        :m_physicalDevice{VK_NULL_HANDLE}, m_device{VK_NULL_HANDLE}, m_graphicsQueue{VK_NULL_HANDLE}, m_presentQueue{VK_NULL_HANDLE}, m_surface{VK_NULL_HANDLE}
    {}

    void Init(VkInstance instance, VkSurfaceKHR surface)

    {
        m_surface = surface;
        PickPhysicalDevice(instance);
        CreateLogicalDevice();
    }

    void WaitIdle()
    {
        vkDeviceWaitIdle(m_device);
    }

    void Destroy()
    {
        vkDestroyDevice(m_device, nullptr);
    }

    VkDevice Get() const
    {
        return m_device;
    }

    VkPhysicalDevice GetPhysical() const
    {
        return m_physicalDevice;
    }

    VkQueue GraphicsQueue() const
    {
        return m_graphicsQueue;
    }

    VkQueue PresentQueue() const
    {
        return m_presentQueue;
    }

    QueueFamilyIndices GetQueueFamilyIndices() const
    {
        return FindQueueFamilies(m_physicalDevice);
    }

private:
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSurfaceKHR m_surface;
    QueueFamilyIndices m_queueIndices;

    void PickPhysicalDevice(VkInstance instance)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (IsDeviceSuitable(device)) {
                m_physicalDevice = device;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }

    void CreateLogicalDevice() {
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
        m_queueIndices = FindQueueFamilies(m_physicalDevice);


        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector<uint32_t> uniqueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
        };
        std::sort(uniqueFamilies.begin(), uniqueFamilies.end());
        uniqueFamilies.erase(std::unique(uniqueFamilies.begin(), uniqueFamilies.end()), uniqueFamilies.end());

        float queuePriority = 1.0f;
        for (uint32_t family : uniqueFamilies) {
            VkDeviceQueueCreateInfo qInfo{};
            qInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qInfo.queueFamilyIndex = family;
            qInfo.queueCount       = 1;
            qInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(qInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos       = queueCreateInfos.data();
        createInfo.pEnabledFeatures        = &deviceFeatures;
        createInfo.enabledExtensionCount   = 0; // add swap‑chain extensions later if needed

        createInfo.enabledExtensionCount  = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        // Validation layers (assume globals `enableValidationLayers` & `validationLayers`)
        extern const bool enableValidationLayers;
        extern const std::vector<const char*> validationLayers;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        // Retrieve queues
        vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    }

    bool IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        return indices.IsComplete() && extensionsSupported;
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; ++i) {
            const auto& qFamily = queueFamilies[i];

            // Graphics support
            if (qFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            // Presentation support – query per‑family
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.IsComplete())
            {
                break;
            }
        }

        return indices;
    }


};