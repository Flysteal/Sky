#pragma once
#include "Window.h"
#include "Vulkan.h"
#include <cstddef>
#include <vulkan/vulkan_core.h>
#include "DebugUtils.h"
#include "Device.h"
#include "Surface.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "Renderer.h"
#include <iostream>

namespace Visuals
{
    struct Visuals
    {
        Visuals()
            :m_window(nullptr),
            m_height(600), m_width(800),
            m_name("SkyLands"),
            m_instance{VK_NULL_HANDLE},
            m_debugMessenger{VK_NULL_HANDLE},
            m_physicalDevice{VK_NULL_HANDLE},
            m_device{VK_NULL_HANDLE},
            m_graphicsQueue{VK_NULL_HANDLE},
            m_surface{VK_NULL_HANDLE},
            m_swapChain{VK_NULL_HANDLE},
            // m_swapChainImageFormat{VK_NULL_HANDLE},
            m_swapChainExtent{m_height, m_width},
            m_graphicsPipeline{VK_NULL_HANDLE},
            m_commandPool{VK_NULL_HANDLE},
            m_commandBuffer{VK_NULL_HANDLE},
            m_imageAvailableSemaphore{VK_NULL_HANDLE},
            m_renderFinishedSemaphore{VK_NULL_HANDLE},
            m_inFlightFence{VK_NULL_HANDLE}
        {
            Create();
            Loop();
            Destroy();
        }

    private:
        GLFWwindow* m_window;
        uint32_t m_height;
        uint32_t m_width;
        const char* m_name;
        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice         m_physicalDevice;
        VkDevice                 m_device;
        VkQueue                  m_graphicsQueue;
        VkSurfaceKHR             m_surface;
        VkQueue                  m_presentQueue;
        VkSwapchainKHR           m_swapChain;
        std::vector<VkImage>     m_swapChainImages;
        VkFormat                 m_swapChainImageFormat;
        VkExtent2D               m_swapChainExtent;
        std::vector<VkImageView> m_swapChainImageViews;
        VkPipelineLayout         m_pipelineLayout;
        VkRenderPass             m_renderPass;
        VkPipeline               m_graphicsPipeline;
        std::vector<VkFramebuffer> m_swapChainFramebuffers;
        VkCommandPool            m_commandPool;
        VkCommandBuffer          m_commandBuffer;
        VkSemaphore              m_imageAvailableSemaphore;
        VkSemaphore              m_renderFinishedSemaphore;
        VkFence                  m_inFlightFence;
        const int MAX_FRAMES_IN_FLIGHT = 2; // to do


        void Create()
        {
            Glfw::Create();
            Window::Create(m_window, m_width, m_height, m_name);
            DebugUtils::CheckSupport(DebugUtils::validationLayers);
            Instance::Create(m_instance, m_name, DebugUtils::validationLayers);
            DebugUtils::Create(m_instance, m_debugMessenger);
            Surface::Create(m_window, m_instance, m_surface);
            PhysicalDevice::Pick(m_instance, m_physicalDevice, m_surface);
            LogicalDevice::Create(m_physicalDevice, m_device, DebugUtils::validationLayers, m_graphicsQueue, m_presentQueue, m_surface);
            SwapChain::Create(m_swapChain, m_physicalDevice, m_device, m_surface, m_window, m_swapChainImages, m_swapChainImageFormat, m_swapChainExtent);
            ImageViews::Create(m_device, m_swapChainImageViews, m_swapChainImages, m_swapChainImageFormat);
            RenderPasses::Create(m_device, m_renderPass, m_swapChainImageFormat);
            GraphicsPipeline::Create(m_graphicsPipeline, m_device, m_swapChainExtent, m_pipelineLayout, m_renderPass);
            Buffers::Create(m_device, m_swapChainFramebuffers, m_swapChainImageViews, m_renderPass, m_swapChainExtent);
            CommandPool::Create(m_device, m_physicalDevice, m_surface, m_commandPool);
            CommandBuffer::Create(m_device, m_commandPool, m_commandBuffer);
            SyncObjects::Create(m_device, m_imageAvailableSemaphore, m_renderFinishedSemaphore, m_inFlightFence);
        }

        void Loop()
        {
            while (!glfwWindowShouldClose(m_window))
            {
                glfwPollEvents();
                Draw::Frame(m_device, m_inFlightFence, m_swapChain, m_imageAvailableSemaphore, m_commandBuffer, m_commandPool, m_renderPass, m_swapChainFramebuffers, m_swapChainExtent, m_graphicsPipeline, m_renderFinishedSemaphore, m_graphicsQueue, m_presentQueue);
            }

            vkDeviceWaitIdle(m_device);
        }

        void Destroy()
        {
            SyncObjects::Destroy(m_device, m_imageAvailableSemaphore, m_renderFinishedSemaphore, m_inFlightFence);
            CommandPool::Destoy(m_device, m_commandPool);
            Buffers::Destroy(m_device, m_swapChainFramebuffers);
            GraphicsPipeline::Destroy(m_device, m_graphicsPipeline, m_pipelineLayout);
            RenderPasses::Destroy(m_device, m_renderPass);
            ImageViews::Destroy(m_device, m_swapChainImageViews);
            SwapChain::Destroy(m_swapChain, m_device);
            Surface::Destroy(m_instance, m_surface);
            LogicalDevice::Destroy(m_device);
            // PhysicalDevice::
            DebugUtils::Destroy(m_instance, m_debugMessenger);
            Instance::Destroy(m_instance);
            Window::Destroy(m_window);
            Glfw::Destroy();
        }
    };
}