#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Visuals
{
    namespace Glfw
    {
        void Create()
        {
            if (!glfwInit())
            {
                throw std::runtime_error("Failed to create GLFW instance !");
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  
        }

        void Destroy()
        {
            glfwTerminate();
        }
    }

    namespace Window
    {
        void Create(GLFWwindow* window, uint32_t height, uint32_t width, const char* name)
        {
            window = glfwCreateWindow(width, height, name, nullptr, nullptr);

            if (!window)
            {
                throw std::runtime_error("Failed to create WINDOW instance !");
            }
        }

        void Destroy(GLFWwindow* window)
        {
            glfwDestroyWindow(window);
        } 
    }
}