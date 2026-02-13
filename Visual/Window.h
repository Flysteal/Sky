#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

struct Window
{
    Window(const char* name, uint32_t width, uint32_t height)
        :m_window(nullptr)
    {
        if (!glfwInit())
        {
            glfwTerminate();
            throw std::runtime_error("Failed to Init glfw !");
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);

        if (!m_window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to Init window !");
            return;
        }

        glfwMakeContextCurrent(m_window);
    }

    void Destroy()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    GLFWwindow* Get() const
    {
        return m_window;
    }

private:
    GLFWwindow* m_window;
};