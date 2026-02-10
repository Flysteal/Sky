#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
public:
    Window()
        :m_window(nullptr)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to init GLFW !!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   
    }

    void Init(const uint32_t& width , const uint32_t& height)
    {
        m_window = glfwCreateWindow(width, height, "SkyLands", nullptr, nullptr);

        if (!m_window)
        {
            throw std::runtime_error("Failed to init WINDOW !!");
        }
    }

    GLFWwindow* Get()
    {
        return m_window;
    }

    void Destroy()
    {
        glfwDestroyWindow(m_window);
    }

    void Terminate()
    {
        glfwTerminate();
    }

private:
    GLFWwindow* m_window;
};