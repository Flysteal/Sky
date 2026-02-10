#pragma once
#include <iostream>
#include "Window.h"
#include "Vulkan.h"
#include "ValidationLayers.h"
#include "Device.h"
#include "Surface.h"
#include "Swapchain.h"
#include "ImageViews.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "CommandBuffers.h"
#include "Renderer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 5;

class App
{
public:

    void Run()
    {
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:
    void InitVulkan() {
        std::cout << "InitVulkan\n";

        m_Window.Init(WIDTH, HEIGHT);
        m_ValidationLayers.Init();
        m_Vulkan.Init();
        m_ValidationLayers.SetupDebugMessenger(m_Vulkan.Get());
        m_Surface.Init(m_Vulkan.Get(), m_Window.Get());
        m_Device.Init(m_Vulkan.Get(), m_Surface.Get());
        m_Swapchain.Init(m_Device.Get(), m_Device.GetPhysical(), m_Surface.Get(), m_Device.GraphicsQueue(), m_Device.PresentQueue());
        m_ImageViews.Init(m_Device.Get(), m_Swapchain.GetImages(), m_Swapchain.GetImageFormat());
        m_RenderPass.Init(m_Device.Get(), m_Swapchain.GetImageFormat());
        m_Pipeline.Init(m_Device.Get(), m_RenderPass.Get());

        m_Swapchain.CreateFramebuffers(m_RenderPass.Get());
        m_CommandBuffers.Init(m_Device, m_RenderPass.Get(), m_Swapchain.GetFramebuffers(), m_Swapchain.GetExtent(), m_Pipeline.Get(), m_Surface.Get(), MAX_FRAMES_IN_FLIGHT);

        m_Renderer.Init(
            m_Device.Get(),
            m_Swapchain.Get(),
            static_cast<uint32_t>(m_Swapchain.GetFramebuffers().size()),
            m_CommandBuffers,
            m_Device.GraphicsQueue(),
            m_Device.PresentQueue(),
            MAX_FRAMES_IN_FLIGHT
        );
    }

    void MainLoop() {
        std::cout << "MainLoop\n";
        int fps = 0;
        int time = 0;

        while(!glfwWindowShouldClose(m_Window.Get()))
        {
            glfwPollEvents();
            m_Renderer.Draw();

            if (glfwGetTime() > time)
            {
                std::cout << fps << '\n';
                fps = 0;
                time = glfwGetTime() + 1;
            }

            fps++;
        }
        m_Device.WaitIdle();
    }

    void Cleanup() {
        std::cout << "Cleanup\n";
        m_Renderer.Destroy();
        m_CommandBuffers.Destroy();
        m_Pipeline.Destroy();
        m_RenderPass.Destroy();
        m_ImageViews.Destroy();
        m_Swapchain.Destroy();
        m_Device.Destroy();
        m_Surface.Destroy();
        m_ValidationLayers.Destroy(m_Vulkan.Get());
        m_Vulkan.Destroy();
        m_Window.Destroy();
        m_Window.Terminate();
    }

    Window m_Window;
    Vulkan m_Vulkan;
    ValidationLayers m_ValidationLayers;
    Device m_Device;
    Surface m_Surface;
    Swapchain m_Swapchain;
    ImageViews m_ImageViews;
    Pipeline m_Pipeline;
    RenderPass m_RenderPass;
    CommandBuffers m_CommandBuffers;
    Renderer m_Renderer;

};