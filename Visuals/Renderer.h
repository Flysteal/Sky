#pragma once

#include "SwapChain.h"
#include <vulkan/vulkan_core.h>

namespace Visuals
{
    namespace Draw
    {
        void Frame(VkDevice& device, VkFence& inFlightFence, VkSwapchainKHR& swapChain, VkSemaphore& imageAvailableSemaphore, VkCommandBuffer& commandBuffer, VkCommandPool& commandPool, VkRenderPass& renderPass, std::vector<VkFramebuffer>& swapChainFramebuffers, VkExtent2D& swapChainExtent, VkPipeline& graphicsPipeline, VkSemaphore& renderFinishedSemaphore, VkQueue& graphicsQueue, VkQueue& presentQueue)
        {
            vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
            vkResetFences(device, 1, &inFlightFence);

            uint32_t imageIndex;
            vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

            vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
            CommandBuffer::Record(commandPool, commandBuffer, imageIndex, renderPass, swapChainFramebuffers, swapChainExtent, graphicsPipeline);

            VkSubmitInfo submitInfo{};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = waitSemaphores;
            submitInfo.pWaitDstStageMask    = waitStages;

            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &commandBuffer;

            VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = signalSemaphores;

            if (VK_SUCCESS != vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence))
            {
                throw std::runtime_error("Failed to submit draw command buffer !");
            }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = signalSemaphores;

            VkSwapchainKHR swapChains[] = {swapChain};
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = swapChains;

            presentInfo.pImageIndices      = &imageIndex;

            vkQueuePresentKHR(presentQueue, &presentInfo);
        }
    }

    namespace SyncObjects
    {
        void Create(VkDevice& device, VkSemaphore& imageAvailableSemaphore, VkSemaphore& renderFinishedSemaphore, VkFence& inFlightFence)
        {
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


            if (VK_SUCCESS != vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) ||
                VK_SUCCESS != vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) ||
                VK_SUCCESS != vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence))
            {
                throw std::runtime_error("Failed to create semaphores !");
            }
        }

        void Destroy(VkDevice& device, VkSemaphore& imageAvailableSemaphore, VkSemaphore& renderFinishedSemaphore, VkFence& inFlightFence)
        {
            vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
            vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
            vkDestroyFence(device, inFlightFence, nullptr);
        }
    }
}