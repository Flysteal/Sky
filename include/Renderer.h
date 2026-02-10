#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include "CommandBuffers.h"

class Renderer
{
public:
    Renderer() = default;

    void Init(
        VkDevice device,
        VkSwapchainKHR swapChain,
        uint32_t swapchainImageCount,
        CommandBuffers& commandBuffers,
        VkQueue graphicsQueue,
        VkQueue presentQueue,
        uint32_t maxFramesInFlight)
    {
        m_device = device;
        m_swapChain = swapChain;
        m_commandBuffers = &commandBuffers;
        m_graphicsQueue = graphicsQueue;
        m_presentQueue = presentQueue;
        m_maxFramesInFlight = maxFramesInFlight;

        // Per-frame sync
        m_imageAvailableSemaphores.resize(maxFramesInFlight);
        m_inFlightFences.resize(maxFramesInFlight);

        // 🔑 Per-image sync (THIS FIXES THE ERROR)
        m_renderFinishedSemaphores.resize(swapchainImageCount);
        m_imagesInFlight.resize(swapchainImageCount, VK_NULL_HANDLE);

        CreateSyncObjects(swapchainImageCount);
    }

    void Draw()
    {
        // Wait for CPU-GPU sync for this frame
        vkWaitForFences(
            m_device,
            1,
            &m_inFlightFences[m_currentFrame],
            VK_TRUE,
            UINT64_MAX);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(
            m_device,
            m_swapChain,
            UINT64_MAX,
            m_imageAvailableSemaphores[m_currentFrame],
            VK_NULL_HANDLE,
            &imageIndex);

        // 🔑 Wait if this swapchain image is still in flight
        if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(
                m_device,
                1,
                &m_imagesInFlight[imageIndex],
                VK_TRUE,
                UINT64_MAX);
        }

        // Mark image as now owned by this frame
        m_imagesInFlight[imageIndex] =
            m_inFlightFences[m_currentFrame];

        vkResetFences(
            m_device,
            1,
            &m_inFlightFences[m_currentFrame]);

        // Record commands
        m_commandBuffers->Record(m_currentFrame, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {
            m_imageAvailableSemaphores[m_currentFrame]
        };
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        const VkCommandBuffer cmd =
            m_commandBuffers->Get()[m_currentFrame];
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        // 🔑 SIGNAL PER-SWAPCHAIN-IMAGE SEMAPHORE
        VkSemaphore signalSemaphores[] = {
            m_renderFinishedSemaphores[imageIndex]
        };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(
                m_graphicsQueue,
                1,
                &submitInfo,
                m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapChain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(m_presentQueue, &presentInfo);

        m_currentFrame =
            (m_currentFrame + 1) % m_maxFramesInFlight;
    }

    void Destroy()
    {
        for (uint32_t i = 0; i < m_imageAvailableSemaphores.size(); i++)
        {
            vkDestroySemaphore(
                m_device,
                m_imageAvailableSemaphores[i],
                nullptr);

            vkDestroyFence(
                m_device,
                m_inFlightFences[i],
                nullptr);
        }

        for (VkSemaphore sem : m_renderFinishedSemaphores)
        {
            vkDestroySemaphore(m_device, sem, nullptr);
        }
    }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;

    CommandBuffers* m_commandBuffers = nullptr;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

    // Per-frame
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkFence> m_inFlightFences;

    // 🔑 Per-image
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_imagesInFlight;

    uint32_t m_maxFramesInFlight = 0;
    uint32_t m_currentFrame = 0;

private:
    void CreateSyncObjects(uint32_t swapchainImageCount)
    {
        VkSemaphoreCreateInfo semInfo{};
        semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Per-frame
        for (uint32_t i = 0; i < m_maxFramesInFlight; i++)
        {
            if (vkCreateSemaphore(
                    m_device,
                    &semInfo,
                    nullptr,
                    &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(
                    m_device,
                    &fenceInfo,
                    nullptr,
                    &m_inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create frame sync objects");
            }
        }

        // 🔑 Per-image
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            if (vkCreateSemaphore(
                    m_device,
                    &semInfo,
                    nullptr,
                    &m_renderFinishedSemaphores[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image semaphores");
            }
        }
    }
};
