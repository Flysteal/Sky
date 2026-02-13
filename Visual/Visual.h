#pragma once

#include "Vulkan/Vulkan.h"
#include "Visual/Window.h"
#include <cstdint>

struct VisualInfo
{
    bool debug       = false;
    const char* name = "SkyLands";
    uint32_t width   = 800;
    uint32_t height  = 600;
};

struct Visual
{
    Visual(const VisualInfo& info)
        :m_info(info),
        m_Window(m_info.name, m_info.width, m_info.height),
        m_Vulkan(m_info.debug, m_Window.Get(), m_info.name, m_info.width, m_info.height)
    {
        
    }

    ~Visual()
    {
        m_Vulkan.Destroy();
        m_Window.Destroy();
    }

private:
    const VisualInfo& m_info;
    Window m_Window;
    Vulkan m_Vulkan;

};