#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include "configuration.hpp"

#include <glm/glm.hpp>

namespace vulkanite::renderer {
    class Image;
    class Queue;

    struct ImageViewCreateInfo {
        Image& image;
        ImageViewType type;
        Flags aspectFlags;

        std::uint32_t baseMipLevel;
        std::uint32_t levelCount;
        std::uint32_t baseArrayLayer;
        std::uint32_t layerCount;
    };

    class ImageView {
    public:
        ImageView(const ImageViewCreateInfo& createInfo);
        ~ImageView();

        ImageViewType getType() const;
        std::uint32_t getBaseMipLevel() const;
        std::uint32_t getLevelCount() const;
        std::uint32_t getBaseArrayLayer() const;
        std::uint32_t getLayerCount() const;

    private:
        VkImage image_ = nullptr;
        VkDevice device_ = nullptr;
        VkImageView imageView_ = nullptr;
        VkImageViewType imageViewType_ = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

        glm::uvec3 extent_;

        std::uint32_t baseMipLevel_ = 0;
        std::uint32_t levelCount_ = 0;
        std::uint32_t baseArrayLayer_ = 0;
        std::uint32_t layerCount_ = 0;

        static VkImageViewType mapType(ImageViewType type);
        static ImageViewType reverseMapType(VkImageViewType type);

        friend class Framebuffer;
        friend class DescriptorPool;
    };

    struct ImageMemoryBarrier {
        Image& image;
        Queue* sourceQueue;
        Queue* destinationQueue;

        std::uint32_t baseMipLevel;
        std::uint32_t mipLevelCount;
        std::uint32_t baseArrayLayer;
        std::uint32_t arrayLayerCount;

        ImageLayout oldLayout;
        ImageLayout newLayout;

        Flags aspectMask;
        Flags sourceAccessFlags;
        Flags destinationAccessFlags;
    };
}

#include "detail/image_view.inl"

#endif