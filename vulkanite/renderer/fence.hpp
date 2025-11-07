#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <cstdint>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;

    using Flags = std::uint32_t;

    struct FenceCreateInfo {
        Device& device;
        Flags createFlags;
    };

    class Fence {
    public:
        Fence(const FenceCreateInfo& createInfo);
        ~Fence();

        bool signalled();

    private:
        VkFence fence_ = nullptr;
        Device* device_ = nullptr;

        friend class Device;
        friend class Queue;
    };
}

#include "detail/fence.inl"

#endif