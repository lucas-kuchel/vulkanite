#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include "configuration.hpp"

#include <cstdint>
#include <span>

#include <glm/glm.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Queue;
    class Device;
    class Instance;

    struct ImageCreateInfo {
        Device& device;
        ImageType type;
        ImageFormat format;
        MemoryType memoryType;
        Flags usageFlags;

        glm::uvec3 extent;

        std::uint32_t sampleCount;
        std::uint32_t mipLevels;
        std::uint32_t arrayLayers;
    };

    struct ImageMapping {
        std::span<std::uint8_t> data;

        std::uint64_t offset = 0;
        std::uint64_t alignedSize = 0;
        std::uint64_t alignedOffset = 0;
    };

    class Image {
    public:
        void create(const ImageCreateInfo& createInfo);
        void destroy();

        ImageMapping map(std::uint64_t sizeBytes, std::uint64_t offsetBytes);
        void unmap(ImageMapping& mapping);

        bool canBeMapped() const;
        ImageFormat getFormat() const;
        ImageType getType() const;
        std::uint64_t getSize() const;
        glm::uvec3 getExtent() const;
        std::uint32_t getSampleCount() const;
        std::uint32_t getMipLevels() const;
        std::uint32_t getArrayLayers() const;

    private:
        VkImage image_ = nullptr;
        VmaAllocation allocation_ = nullptr;
        Device* device_ = nullptr;

        glm::uvec3 extent_;

        std::uint32_t sampleCount_;
        std::uint32_t mipLevels_;
        std::uint32_t arrayLayers_;

        VkImageType type_ = VK_IMAGE_TYPE_MAX_ENUM;
        VkFormat format_ = VK_FORMAT_MAX_ENUM;

        bool isHostVisible_ = false;
        bool isHostCoherent_ = false;

        std::uint64_t size_ = 0;

        static VkFormat mapFormat(ImageFormat format, renderer::Instance& instance);
        static ImageFormat reverseMapFormat(VkFormat format);

        static VkImageType mapType(ImageType type);
        static ImageType reverseMapType(VkImageType type);

        friend class Swapchain;
        friend class RenderPass;
        friend class Framebuffer;
        friend class ImageView;
        friend class CommandBuffer;
    };

    struct BufferImageCopyRegion {
        std::uint64_t bufferOffset;
        std::uint32_t bufferRowLength;
        std::uint32_t bufferImageHeight;
        std::uint32_t mipLevel;
        std::uint32_t baseArrayLayer;
        std::uint32_t arrayLayerCount;

        glm::ivec3 imageOffset;
        glm::uvec3 imageExtent;

        Flags imageAspectMask;
    };
}

#include "detail/image.inl"

#endif