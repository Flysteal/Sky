#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <optional>

#include "Device.h"

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain {
public:
    Swapchain()
        :m_device{VK_NULL_HANDLE}, m_physicalDevice{VK_NULL_HANDLE}, m_surface{VK_NULL_HANDLE}, m_graphicsQueue{VK_NULL_HANDLE}, m_imageFormat{VK_FORMAT_UNDEFINED}
    {}

    void Init(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue graphicsQueue, VkQueue presentQueue)
    {
        m_device = device;
        m_physicalDevice = physicalDevice;
        m_surface = surface;
        m_graphicsQueue = graphicsQueue;
        m_presentQueue = presentQueue;

        CreateSwapChain();
        CreateImageViews();
    }

    void CreateFramebuffers(VkRenderPass renderPass)
    {
        m_Framebuffers.resize(m_imageViews.size());

        for (size_t i = 0; i < m_imageViews.size(); i++) {
            VkImageView attachments[] = {
                m_imageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass      = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments    = attachments;
            framebufferInfo.width           = m_extent.width;
            framebufferInfo.height          = m_extent.height;
            framebufferInfo.layers          = 1;

            if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void Destroy()
    {
        for (auto framebuffer : m_Framebuffers) {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }

        for (auto view : m_imageViews) {
            vkDestroyImageView(m_device, view, nullptr);
        }
        if (m_swapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
        }
    }

    VkSwapchainKHR Get() const
    {
        return m_swapChain;
    }
    const std::vector<VkImageView>& GetImageViews() const
    {
        return m_imageViews;
    }
    const std::vector<VkImage>& GetImages() const
    {
        return m_images;
    }
    const std::vector<VkFramebuffer>& GetFramebuffers() const
    {
        return m_Framebuffers;
    }
    VkFormat GetImageFormat() const
    {
        return m_imageFormat;
    }
    VkExtent2D GetExtent() const
    {
        return m_extent;
    }


private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;
    VkSurfaceKHR m_surface;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkFormat m_imageFormat;
    VkExtent2D m_extent{};
    std::vector<VkFramebuffer> m_Framebuffers;


    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                                  VkSurfaceKHR surface) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available) {
        for (const auto& fmt : available) {
            if (fmt.format == VK_FORMAT_B8G8R8A8_SRGB &&
                fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return fmt;
            }
        }
        return available[0];
    }

    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& available) {
        for (const auto& mode : available) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& caps) {
        if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return caps.currentExtent;
        }
        // Replace with actual framebuffer size from your window system
        VkExtent2D actualExtent = {800, 600};

        actualExtent.width  = std::clamp(actualExtent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, caps.minImageExtent.height, caps.maxImageExtent.height);
        return actualExtent;
    }

    void CreateSwapChain() {
        auto support = QuerySwapChainSupport(m_physicalDevice, m_surface);
        if (support.formats.empty() || support.presentModes.empty()) {
            throw std::runtime_error("Insufficient swap‑chain support");
        }

        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(support.formats);
        VkPresentModeKHR presentMode     = ChoosePresentMode(support.presentModes);
        VkExtent2D extent                = ChooseExtent(support.capabilities);

        uint32_t imageCount = support.capabilities.minImageCount + 1;
        if (support.capabilities.maxImageCount > 0 &&
            imageCount > support.capabilities.maxImageCount) {
            imageCount = support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = m_surface;
        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Determine sharing mode based on queue families
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
        uint32_t queueFamilyIndices[] = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
        };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform   = support.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = presentMode;
        createInfo.clipped        = VK_TRUE;
        createInfo.oldSwapchain   = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain");
        }

        m_imageFormat = surfaceFormat.format;
        m_extent      = extent;

        // Retrieve images
        vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
        m_images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_images.data());
    }

    void CreateImageViews() {
        m_imageViews.resize(m_images.size());

        for (size_t i = 0; i < m_images.size(); ++i) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image    = m_images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format   = m_imageFormat;

            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel   = 0;
            viewInfo.subresourceRange.levelCount     = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image view for swap chain image");
            }
        }
    }

    // You must provide an implementation that matches your existing one.
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
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
