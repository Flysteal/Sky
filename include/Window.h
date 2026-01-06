#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>


namespace sky
{
    struct WindowInfo
    {
        const char* name;
        uint32_t width;
        uint32_t height;
    };
    class Window
    {
    public:
        Window(const WindowInfo& info)
            :m_window{nullptr}
        {
            Init(info);
        }

        GLFWwindow* Get()
        {
            return m_window;
        }

        void Destroy()
        {
            glfwDestroyWindow(m_window);
            glfwTerminate();
        }

    private:
        GLFWwindow* m_window;

        void Init(const WindowInfo& info)
        {
            if(!glfwInit())
            {
                throw std::runtime_error("failed to glfwInit");
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            m_window = glfwCreateWindow(info.width, info.height, info.name, nullptr, nullptr);
            if (nullptr == m_window)
            {
                throw std::runtime_error("failed to crate m_window");
            }
        }
    };
}//sky

#endif//WINDOW_H