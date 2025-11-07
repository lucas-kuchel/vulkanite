#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include "configuration.hpp"

#include <optional>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;

    struct SamplerCreateInfo {
        Device& device;
        Filter minFilter;
        Filter magFilter;
        MipmapMode mipmapMode;
        AddressMode addressModeU;
        AddressMode addressModeV;
        AddressMode addressModeW;
        BorderColour borderColour;

        std::optional<float> maxAnisotropy;
        std::optional<CompareOperation> comparison;

        bool unnormalisedCoordinates;

        float mipLodBias;
        float minLod;
        float maxLod;
    };

    class Sampler {
    public:
        void create(const SamplerCreateInfo& createInfo);
        void destroy();

    private:
        VkSampler sampler_ = nullptr;
        Device* device_ = nullptr;

        friend class DescriptorPool;
    };
}

#include "detail/sampler.inl"

#endif