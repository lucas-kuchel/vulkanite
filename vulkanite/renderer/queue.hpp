#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Surface;
    class Semaphore;
    class Fence;
    class CommandBuffer;

    using Flags = std::uint32_t;

    struct QueueCreateInfo {
        Flags flags;
        Surface* surface;
    };

    struct QueueSubmitInfo {
        Fence& fence;

        std::vector<CommandBuffer> commandBuffers;
        std::vector<Semaphore> waits;
        std::vector<Semaphore> signals;
        std::vector<std::uint32_t> waitFlags;
    };

    class Queue {
    public:
        bool submit(const QueueSubmitInfo& submitInfo);

    private:
        VkQueue queue_ = nullptr;

        std::uint32_t familyIndex_;
        std::uint32_t queueIndex_;

        friend class Device;
        friend class CommandPool;
        friend class CommandBuffer;
        friend class Swapchain;
    };
}

#include "detail/queue.inl"

#endif