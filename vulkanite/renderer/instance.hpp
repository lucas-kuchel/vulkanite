#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    struct InstanceCreateInfo {
        std::string applicationName;

        std::uint32_t applicationVersionMajor;
        std::uint32_t applicationVersionMinor;
        std::uint32_t applicationVersionPatch;

        bool requestDebug;
    };

    class Instance {
    public:
        Instance(const InstanceCreateInfo& createInfo);
        ~Instance();

    private:
        VkInstance instance_ = nullptr;
        VkPhysicalDevice physicalDevice_ = nullptr;

        VkPhysicalDeviceMemoryProperties memoryProperties_;
        VkPhysicalDeviceProperties properties_;

        std::uint32_t apiVersion_ = 0;

        std::vector<VkQueueFamilyProperties> queueFamilyProperties_;
        std::vector<std::uint32_t> queueFamilyOccupations_;

        friend class Device;
        friend class Buffer;
        friend class Image;
        friend class Surface;
        friend class Swapchain;
    };
}

#include "detail/instance.inl"

#endif