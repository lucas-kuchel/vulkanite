#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include "configuration.hpp"

#include <span>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;

    struct BufferCopyRegion {
        std::uint64_t sourceOffsetBytes;
        std::uint64_t destinationOffsetBytes;
        std::uint64_t sizeBytes;
    };

    struct BufferCreateInfo {
        Device& device;
        MemoryType memoryType;
        Flags usageFlags;

        std::uint64_t sizeBytes;
    };

    struct BufferMapping {
        std::span<std::uint8_t> data;

        std::uint64_t offset = 0;
        std::uint64_t alignedSize = 0;
        std::uint64_t alignedOffset = 0;
    };

    class Buffer {
    public:
        void create(const BufferCreateInfo& createInfo);
        void destroy();

        BufferMapping map(std::uint64_t size, std::uint64_t offset);
        void unmap(BufferMapping& mapping);

        std::uint64_t getSize() const;
        bool canBeMapped() const;

        explicit operator bool() {
            return buffer_ && allocation_ && device_;
        }

    private:
        VkBuffer buffer_ = nullptr;
        VmaAllocation allocation_ = nullptr;
        Device* device_ = nullptr;

        bool isHostCoherent_ = false;
        bool isHostVisible_ = false;

        std::uint64_t size_ = 0;

        friend class CommandBuffer;
        friend class DescriptorPool;
    };
}

#include "detail/buffer.inl"

#endif