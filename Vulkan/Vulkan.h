#pragma once

#include <cstring>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "ValidationLayers.h"
#include "Device.h"
#include "SwapChain.h"

struct Vulkan
{
    Vulkan(const bool debug, GLFWwindow* window, const char* name, uint32_t width, uint32_t height)
        :m_debug(debug), m_instance{VK_NULL_HANDLE}, m_ValidationLayers()
    {
        CreateInstance();
        m_ValidationLayers.Init(m_instance);
        CreateSurface(m_instance, window);
        m_Device.Init(m_surface);
        m_Device.PickPhysicalDevice(m_instance);
        m_Device.CreateLogicalDevice();
    }

    void Destroy()
    {
        m_Device.Destroy();
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_ValidationLayers.Destroy(m_instance);

        if (VK_NULL_HANDLE != m_instance)
        {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }
    }

    void CreateInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "SkyLands";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "Sky";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 3);
        appInfo.apiVersion         = VK_API_VERSION_1_3;


        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        std::vector<const char*> layers;

        if (true == m_debug)
        {
            if (false == m_ValidationLayers.CheckValidationLayerSupport(m_validationLayers))
            {
                throw std::runtime_error("Validation layers requested but not available !");
            }

            layers = m_validationLayers;
            extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            m_ValidationLayers.PopulateDebugMessengerCreateInfo(debugCreateInfo);
        }


        VkInstanceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext                   = true == m_debug ? (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo  : nullptr;
        createInfo.flags                   = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        createInfo.pApplicationInfo        = &appInfo;
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount       = static_cast<uint32_t>(layers.size());
        createInfo.ppEnabledLayerNames     = layers.empty() ? nullptr : layers.data();

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to init Vulkan!");
        }
    }

    void CreateSurface(VkInstance instance, GLFWwindow* window)
    {
        if (VK_SUCCESS != glfwCreateWindowSurface(instance, window, nullptr, &m_surface))
        {
            throw std::runtime_error("Failed to create window surface !");
        }
    }

private:
    const bool m_debug;
    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    ValidationLayers m_ValidationLayers;
    Device m_Device;

    const std::vector<const char*> m_validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };
};