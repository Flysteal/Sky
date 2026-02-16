#pragma once

#include "Vulkan.h"
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan.h>
static constexpr bool kDebug = true;

namespace Visuals
{
    namespace DebugUtils
    {

        const std::vector<const char*> validationLayers =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        void CheckSupport(const std::vector<const char*> validationLayers)
        {
            if (false == kDebug)
            {
                return;
            }

            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers)
            {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers)
                {
                    if (strcmp(layerName, layerProperties.layerName) == 0)
                    {
                        layerFound = true;
                        break;
                    }
                }

                if (false == layerFound)
                {
                    throw std::runtime_error("ValidationLayers requested but not available !");
                }
            }
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
        {
            const char* text_color = "\033[0m";
            const char* border_color = "\033[0m";
            const char* text_border_color = "\033[0m";
            const char* reset = "\033[0m";

            /* STRUCTURE: 
            [   ] = border_color
            [ this = text_border_color ]
            []: text_color
            */

            if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT == messageSeverity)
            {
                text_color = "\033[37;1m";
                border_color = "\033[31;1m";
                text_border_color = "\033[31m";
            }
            else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT == messageSeverity)
            {
                text_color = "\033[37m";
                border_color = "\033[33;1m";
                text_border_color = "\033[33m";

                if (VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT == messageType)
                {
                    border_color = "\033[36;1m";
                    text_border_color = "\033[36m";
                }
            }
            else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT == messageSeverity)
            {
                text_color = "\033[37m";
                border_color = "\033[30;1m";
                text_border_color = "\033[37;1m";
            }

            std::cerr << border_color << "[" << reset << text_border_color << pCallbackData->pMessageIdName << reset << border_color << "]" << reset
                      << text_border_color << ": " << reset
                      << text_color << pCallbackData->pMessage << reset << std::endl;

            return VK_FALSE;
        }


        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
        {
            createInfo = {};
            createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.pNext           = nullptr;
            createInfo.flags           = 0;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = DebugCallback;
            createInfo.pUserData       = nullptr; // Optional
        }

        VkResult CreateDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (nullptr != func)
            {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            }
            else
            {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance& instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (nullptr != func)
            {
                func(instance, debugMessenger, pAllocator);
            }
        }

        void Create(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            PopulateDebugMessengerCreateInfo(createInfo);

            if (VK_SUCCESS != CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger))
            {
                throw std::runtime_error("Filed to create debug messenger !");
            }
        }

        void Destroy(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger)
        {
            if (true == kDebug && VK_NULL_HANDLE != debugMessenger)
            {
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
                debugMessenger = VK_NULL_HANDLE;
            }
        }
    }
}