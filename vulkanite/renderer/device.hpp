#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include "queue.hpp"

#include <limits>
#include <span>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Instance;
    class Surface;
    class Pipeline;

    struct PipelineCreateInfo;

    struct DeviceCreateInfo {
        Instance& instance;

        std::vector<QueueCreateInfo> queues;
    };

    class Device {
    public:
        Device(const DeviceCreateInfo& createInfo);
        ~Device();

        bool waitIdle();
        bool waitForFences(const std::vector<Fence>& fences, bool waitAll = true, std::uint32_t timeout = std::numeric_limits<std::uint32_t>::max());
        bool resetFences(const std::vector<Fence>& fences);

        std::vector<Pipeline> createPipelines(const std::vector<PipelineCreateInfo>& createInfos);
        std::span<Queue> getQueues();

    private:
        VkDevice device_ = nullptr;
        VmaAllocator allocator_ = nullptr;
        Instance* instance_ = nullptr;

        std::vector<Queue> queues_;

        friend class CommandPool;
        friend class Buffer;
        friend class ShaderModule;
        friend class Semaphore;
        friend class Fence;
        friend class Sampler;
        friend class Framebuffer;
        friend class DescriptorSetLayout;
        friend class DescriptorPool;
        friend class PipelineLayout;
        friend class Pipeline;
        friend class ImageView;
        friend class Image;
        friend class RenderPass;
        friend class Swapchain;
    };
}

#include "detail/device.inl"

#endif