#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "Device.h"

class CommandBuffers
{
public:
    CommandBuffers() = default;

    void Init(
        Device& device,
        VkRenderPass renderPass,
        const std::vector<VkFramebuffer>& swapChainFramebuffers,
        const VkExtent2D& swapChainExtent,
        VkPipeline graphicsPipeline,
        VkSurfaceKHR surface,
        uint32_t maxFramesInFlight)
    {
        m_device = device.Get();
        m_physicalDevice = device.GetPhysical();
        m_renderPass = renderPass;
        m_swapChainFramebuffers = swapChainFramebuffers;
        m_swapChainExtent = swapChainExtent;
        m_graphicsPipeline = graphicsPipeline;
        m_surface = surface;
        m_maxFramesInFlight = maxFramesInFlight;

        CreateCommandPool();
        CreateCommandBuffers();
    }

    const std::vector<VkCommandBuffer>& Get() const
    {
        return m_commandBuffers;
    }

    void Record(uint32_t frameIndex, uint32_t imageIndex)
    {
        VkCommandBuffer cmd = m_commandBuffers[frameIndex];

        vkResetCommandBuffer(cmd, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapChainExtent;

        VkClearValue clearColor = { {{0.f, 0.f, 0.f, 1.f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

        VkViewport viewport{};
        viewport.width  = static_cast<float>(m_swapChainExtent.width);
        viewport.height = static_cast<float>(m_swapChainExtent.height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_swapChainExtent;
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        vkCmdDraw(cmd, 3, 1, 0, 0);

        vkCmdEndRenderPass(cmd);

        if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer");
        }
    }

    void Destroy()
    {
        vkFreeCommandBuffers(
            m_device,
            m_commandPool,
            static_cast<uint32_t>(m_commandBuffers.size()),
            m_commandBuffers.data()
        );

        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    VkExtent2D m_swapChainExtent{};
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    std::vector<VkCommandBuffer> m_commandBuffers;

    uint32_t m_maxFramesInFlight = 0;

private:
    void CreateCommandPool()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice, m_surface);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool");
        }
    }

    void CreateCommandBuffers()
    {
        m_commandBuffers.resize(m_maxFramesInFlight);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount =
            static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(
                m_device,
                &allocInfo,
                m_commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers");
        }
    }

    QueueFamilyIndices FindQueueFamilies(
        VkPhysicalDevice device,
        VkSurfaceKHR surface) const
    {
        QueueFamilyIndices indices;

        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

        std::vector<VkQueueFamilyProperties> families(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

        for (uint32_t i = 0; i < count; ++i)
        {
            if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = i;

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                device, i, surface, &presentSupport);

            if (presentSupport)
                indices.presentFamily = i;

            if (indices.IsComplete())
                break;
        }

        return indices;
    }
};
