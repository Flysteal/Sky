#ifndef VULKAN_MANAGER_H
#define VULKAN_MANAGER_H

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>

namespace sky
{
    struct VulkanManagerInfo
    {
        const char* AppName;
        const char* EngineName;
        bool enableValidationLayers; /*FOR DDEBUG*/
    };

    class VulkanManager
    {
    public:
        VulkanManager(const VulkanManagerInfo& info)
            :m_info{info}
        {
            CreateInstance();
            SetupDebugMessenger(); /*FOR DEBUG*/
        }

        ~VulkanManager()
        {
            CleanupDebugMessenger(); /*FOR DEBUG*/
            CleanupInstance();
        }

        VkInstance GetInstance() const
        {
            return m_instance;
        }

    private:
        VulkanManagerInfo m_info;
        VkInstance m_instance = VK_NULL_HANDLE;
        DebugMessenger m_DebugMessenger;

        std::vector<const char*> GetRequiredExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (m_info.enableValidationLayers) /*FOR DDEBUG*/
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return extensions;
        }

        void CreateInstance()
        {
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = m_info.AppName;
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = m_info.EngineName;
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_3;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            // TODO: mac os
            auto extensions = GetRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (m_info.enableValidationLayers) /*FOR DDEBUG*/
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
                m_DebugMessenger.PopulateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = &debugCreateInfo;
            }
            else
            {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }

            if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create instance!");
            }
        }

        void CleanupInstance()
        {
            if (m_instance != VK_NULL_HANDLE)
            {
                vkDestroyInstance(m_instance, nullptr);
                m_instance = VK_NULL_HANDLE;
            }
        }

        void SetupDebugMessenger() /*FOR DEBUG*/
        {
            m_DebugMessenger.Setup(m_instance, m_info.enableValidationLayers);
        }

        void CleanupDebugMessenger() /*FOR DEBUG*/
        {
            m_DebugMessenger.Cleanup();
        }
    };
}

#endif//VULKAN_MANAGER_H