#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <vector>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;
    class ImageView;
    class RenderPass;

    struct FramebufferCreateInfo {
        Device& device;
        RenderPass& renderPass;

        std::vector<ImageView> imageViews;
    };

    class Framebuffer {
    public:
        Framebuffer(const FramebufferCreateInfo& createInfo);
        ~Framebuffer();

    private:
        VkFramebuffer framebuffer_ = nullptr;
        Device* device_ = nullptr;

        friend class CommandBuffer;
    };
}

#include "detail/framebuffer.inl"

#endif