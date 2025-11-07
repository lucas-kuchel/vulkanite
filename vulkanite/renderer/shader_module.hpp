#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <cstdint>
#include <span>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;

    struct ShaderModuleCreateInfo {
        Device& device;

        std::span<std::uint32_t> data;
    };

    class ShaderModule {
    public:
        ShaderModule(const ShaderModuleCreateInfo& createInfo);
        ~ShaderModule();

    private:
        VkShaderModule module_ = nullptr;
        Device* device_ = nullptr;

        friend class Device;
    };
}

#include "detail/shader_module.inl"

#endif