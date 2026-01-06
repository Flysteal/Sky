#ifndef VALIDATION_LAYERS_H
#define VALIDATION_LAYERS_H

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <cstring>

namespace sky
{

    const std::vector<const char*> validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    struct ValidationLayersInfo
    {
        bool enableValidationLayers; /*FOR DDEBUG*/
    };

    class ValidationLayers
    {
    public:
        explicit ValidationLayers(const ValidationLayersInfo& info)
            : m_enabled(info.enableValidationLayers)
        {
            if (m_enabled && !CheckSupport())
            {
                throw std::runtime_error("Validation layers not available");
            }
        }

        bool enabled() const
        {
            return m_enabled;
        }

    private:
        bool m_enabled;

        bool CheckSupport()
        {
            uint32_t count = 0;
            vkEnumerateInstanceLayerProperties(&count, nullptr);

            std::vector<VkLayerProperties> layers(count);
            vkEnumerateInstanceLayerProperties(&count, layers.data());

            for (const char* name : validationLayers)
            {
                bool found = false;
                for (const auto& layer : layers)
                {
                    if (std::strcmp(name, layer.layerName) == 0)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    return false;
                }
            }
            return true;
        }
    };
}//sky

#endif//VALIDATION_LAYERS_H