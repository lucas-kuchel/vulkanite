#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "../window/window.hpp"

namespace vulkanite::renderer {
    class Instance;

    struct SurfaceCreateInfo {
        Instance& instance;
        vulkanite::window::Window& window;
    };

    class Surface {
    public:
        Surface(const SurfaceCreateInfo& createInfo);
        ~Surface();

        glm::uvec2 getExtent() const;

    private:
        VkSurfaceKHR surface_ = nullptr;
        Instance* instance_ = nullptr;
        vulkanite::window::Window* window_ = nullptr;

        friend class Device;
        friend class Swapchain;
    };
}

#include "detail/surface.inl"

#endif