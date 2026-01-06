#ifndef APPLICATION_H
#define APPLICATION_H

#include "ValidationLayers.h"
#include "DebugMessenger.h"
#include "Device.h"
#include "Window.h"
#include "VulkanManager.h"

namespace sky
{
    struct ApplicationInfo
    {
        const char* name = "Sky";
        uint32_t width = 800;
        uint32_t height = 600;
        bool enableValidationLayers = false; /*FOR DDEBUG*/
    };
    class Application
    {
    public:
        explicit Application(const ApplicationInfo& info = ApplicationInfo())
            : m_validationLayers({info.enableValidationLayers}),
              m_window({info.name, info.width, info.height}),
              m_vulkan({info.name, "SkyLands", info.enableValidationLayers}),
              m_Device({info.enableValidationLayers}, m_vulkan.GetInstance())
        {}

        void Run()
        {
            // main loop later
            while (!glfwWindowShouldClose(m_window.Get()))
            {
                if (1.0 < glfwGetTime()) { break; } /*FOR DEBUG*/
                glfwPollEvents();
            }
        }

    private:
        ValidationLayers m_validationLayers;
        Window m_window;
        VulkanManager m_vulkan;
        Device m_Device;
    };
}

#endif//APPLICATION_H