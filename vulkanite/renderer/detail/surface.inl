#pragma once

#include "../instance.hpp"
#include "../surface.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void vulkanite::renderer::Surface::create(const SurfaceCreateInfo& createInfo) {
    if (glfwCreateWindowSurface(createInfo.instance.instance_, createInfo.window.getHandle(), nullptr, &surface_) != VK_SUCCESS) {
        surface_ = nullptr;
    }
    else {
        window_ = &createInfo.window;
        instance_ = &createInfo.instance;
    }
}

void vulkanite::renderer::Surface::destroy() {
    if (surface_) {
        vkDestroySurfaceKHR(instance_->instance_, surface_, nullptr);

        surface_ = nullptr;
    }
}

glm::uvec2 vulkanite::renderer::Surface::getExtent() const {
    return window_->getExtent();
}