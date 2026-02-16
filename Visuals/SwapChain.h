#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Device.h"

namespace Visuals
{
    namespace SwapChain
    {
        struct SupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        SupportDetails QuerySupport(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface)
        {
            SupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
            if (0 != formatCount)
            {
                details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
            }

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
            if (0 != presentModeCount)
            {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
            }

            return details;
        }

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
        {
            for (const auto& availableFormat : availableFormats)
            {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    return availableFormat;
                }
            }

            return availableFormats[0];
        }

        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
        {
            for (const auto& availablePresentMode : availablePresentModes)
            {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    return availablePresentMode;
                }
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow*& window)
        {
            if (std::numeric_limits<uint32_t>::max() != capabilities.currentExtent.width)
            {
                return capabilities.currentExtent;
            }
            else
            {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                VkExtent2D actualExtent =
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                };

                actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        void Create(VkSwapchainKHR& swapChain, VkPhysicalDevice& physicalDevice, VkDevice& device, VkSurfaceKHR& surface, GLFWwindow*& window, std::vector<VkImage>& swapChainImages, VkFormat& swapChainImageFormat, VkExtent2D& swapChainExtent)
        {
            SupportDetails swapChainSupport = QuerySupport(physicalDevice, surface);

            VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
            VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
            VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

            uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

            if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            {
                imageCount = swapChainSupport.capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface          = surface;
            createInfo.minImageCount    = imageCount;
            createInfo.imageFormat      = surfaceFormat.format;
            createInfo.imageColorSpace  = surfaceFormat.colorSpace;
            createInfo.imageExtent      = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            PhysicalDevice::QueueFamilyIndices indices = PhysicalDevice::FindQueueFamilies(physicalDevice, surface);
            uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

            if (indices.graphicsFamily != indices.presentFamily)
            {
                createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices   = queueFamilyIndices;
            }
            else
            {
                createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0; // Optional
                createInfo.pQueueFamilyIndices   = nullptr; // Optional
            }

            createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
            createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode      = presentMode;
            createInfo.clipped          = VK_TRUE;
            createInfo.oldSwapchain     = VK_NULL_HANDLE;

            if (VK_SUCCESS != vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain))
            {
                throw std::runtime_error("Failed to create swap chain !");
            }

            vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
            swapChainImages.resize(imageCount);
            vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

            swapChainImageFormat = surfaceFormat.format;
            swapChainExtent = extent;
        }

        void Destroy(VkSwapchainKHR& swapChain, VkDevice& device)
        {
            vkDestroySwapchainKHR(device, swapChain, nullptr);
        }
    }

    namespace ImageViews
    {
        void Create(VkDevice& device, std::vector<VkImageView>& swapChainImageViews, std::vector<VkImage>& swapChainImages, VkFormat& swapChainImageFormat)
        {
            swapChainImageViews.resize(swapChainImages.size());

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                VkImageViewCreateInfo viewInfo{};
                viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image    = swapChainImages[i];
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format   = swapChainImageFormat;

                viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

                viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel   = 0;
                viewInfo.subresourceRange.levelCount     = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount     = 1;

                if (VK_SUCCESS != vkCreateImageView(device, &viewInfo, nullptr, &swapChainImageViews[i]))
                {
                    throw std::runtime_error("Failed to create image view for swap chain image");
                }
            }
        }

        void Destroy(VkDevice& device, std::vector<VkImageView>& swapChainImageViews)
        {
           for (auto imageView : swapChainImageViews)
           {
                vkDestroyImageView(device, imageView, nullptr);
            } 
        }
    }

    namespace Buffers
    {
        void Create(VkDevice& device, std::vector<VkFramebuffer>& swapChainFramebuffers , std::vector<VkImageView>& swapChainImageViews, VkRenderPass& renderPass, VkExtent2D& swapChainExtent)
        {
            swapChainFramebuffers.resize(swapChainImageViews.size());

            for (size_t i = 0; i < swapChainImageViews.size(); i++)
            {
                VkImageView attachments[] =
                {
                    swapChainImageViews[i]
                };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass      = renderPass;
                framebufferInfo.attachmentCount = 1;
                framebufferInfo.pAttachments    = attachments;
                framebufferInfo.width           = swapChainExtent.width;
                framebufferInfo.height          = swapChainExtent.height;
                framebufferInfo.layers          = 1;

                if (VK_SUCCESS != vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]))
                {
                    throw std::runtime_error("Failed to create framebuffer !");
                }
            }
        }

        void Destroy(VkDevice& device, std::vector<VkFramebuffer>& swapChainFramebuffers)
        {
            for (auto framebuffer : swapChainFramebuffers)
            {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            }
        }
    }

    namespace CommandPool
    {
        void Create(VkDevice& device, VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface, VkCommandPool& commandPool)
        {
            PhysicalDevice::QueueFamilyIndices queueFamilyIndices = PhysicalDevice::FindQueueFamilies(physicalDevice, surface);

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();


            if (VK_SUCCESS != vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool))
            {
                throw std::runtime_error("Failed to create command pool !");
            }
        }

        void Destoy(VkDevice& device, VkCommandPool& commandPool)
        {
            vkDestroyCommandPool(device, commandPool, nullptr);
        }
    }

    namespace CommandBuffer
    {
        void Create(VkDevice& device, VkCommandPool& commandPool, VkCommandBuffer& commandBuffer)
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool        = commandPool;
            allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            if (VK_SUCCESS != vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer))
            {
                throw std::runtime_error("Failed to allocate command buffers !");
            }
        }

        void Record(VkCommandPool& commandPool, VkCommandBuffer& commandBuffer, uint32_t imageIndex, VkRenderPass& renderPass, std::vector<VkFramebuffer>& swapChainFramebuffers, VkExtent2D& swapChainExtent, VkPipeline& graphicsPipeline)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags            = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (VK_SUCCESS != vkBeginCommandBuffer(commandBuffer, &beginInfo))
            {
                throw std::runtime_error("Failed to begin recording command buffer !");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass        = renderPass;
            renderPassInfo.framebuffer       = swapChainFramebuffers[imageIndex];

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;

            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
            renderPassInfo.clearValueCount   = 1;
            renderPassInfo.pClearValues      = &clearColor;

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            VkViewport viewport{};
            viewport.x        = 0.0f;
            viewport.y        = 0.0f;
            viewport.width    = static_cast<float>(swapChainExtent.width);
            viewport.height   = static_cast<float>(swapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = swapChainExtent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

            vkCmdEndRenderPass(commandBuffer);

            if (VK_SUCCESS != vkEndCommandBuffer(commandBuffer))
            {
                throw std::runtime_error("Failed to record command buffer !");
            }
        }
    }
}