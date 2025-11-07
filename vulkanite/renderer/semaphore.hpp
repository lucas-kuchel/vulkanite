#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;

    class Semaphore {
    public:
        Semaphore(Device& device);
        ~Semaphore();

    private:
        VkSemaphore semaphore_ = nullptr;
        Device* device_ = nullptr;

        friend class Queue;
        friend class Swapchain;
    };
}

#include "detail/semaphore.inl"

#endif