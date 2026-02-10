#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>


class ImageViews {
public:
    ImageViews()
        :m_device{VK_NULL_HANDLE}
    {}

    void Init(VkDevice device, const std::vector<VkImage>& images, VkFormat format) {
        m_device = device;
        m_images = images;
        m_format = format;
        Create();
    }

    /// Release all image views.  Must be called **before** the swap‑chain
    /// or the device is destroyed.
    void Destroy() {
        for (VkImageView view : m_imageViews) {
            vkDestroyImageView(m_device, view, nullptr);
        }
        m_imageViews.clear();
    }

    // Accessors
    const std::vector<VkImageView>& GetViews() const { return m_imageViews; }
    const std::vector<VkImage>&     GetImages() const { return m_images; }

private:
    VkDevice m_device;
    VkFormat m_format{};
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    void Create() {
        m_imageViews.resize(m_images.size());

        for (size_t i = 0; i < m_images.size(); ++i) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image    = m_images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format   = m_format;

            // No swizzling – keep the channels as‑is
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            // We only need the color aspect, one mip level, one layer
            viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel   = 0;
            viewInfo.subresourceRange.levelCount     = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image view");
            }
        }
    }
};
