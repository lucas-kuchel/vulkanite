#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;
    class Queue;
    class CommandBuffer;

    struct CommandPoolCreateInfo {
        Device& device;
        Queue& queue;
    };

    class CommandPool {
    public:
        void create(const CommandPoolCreateInfo& createInfo);
        void destroy();

        std::vector<CommandBuffer> allocateCommandBuffers(std::uint32_t count);

        void destroyCommandBuffers(const std::vector<CommandBuffer>& buffers);
        bool resetAllCommandBuffers();

    private:
        VkCommandPool commandPool_ = nullptr;
        Device* device_ = nullptr;
        Queue* queue_ = nullptr;
    };
}

#include "detail/command_pool.inl"

#endif