#pragma once

#include "../subsystem.hpp"
#include "../window.hpp"

#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void vulkanite::window::Subsystem::create() {
    subsystemCount_++;

    if (subsystemCount_ > 1) {
        return;
    }

    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Call failed: vulkanite::window::Subsystem::Subsystem(): Failed to initialise window API");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void vulkanite::window::Subsystem::destroy() {
    subsystemCount_--;

    if (subsystemCount_ == 0) {
        glfwTerminate();
    }
}

void vulkanite::window::Subsystem::pollEvents() {
    glfwPollEvents();
}

void vulkanite::window::Subsystem::awaitEvents() {
    glfwWaitEvents();
}

void vulkanite::window::Subsystem::awaitEventsTimeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

vulkanite::window::Backend vulkanite::window::Subsystem::queryBackend() const {
    switch (glfwGetPlatform()) {
        case GLFW_PLATFORM_COCOA:
            return Backend::COCOA;

        case GLFW_PLATFORM_WAYLAND:
            return Backend::WAYLAND;

        case GLFW_PLATFORM_WIN32:
            return Backend::WIN32;

        case GLFW_PLATFORM_X11:
            return Backend::X11;

        default:
            throw std::runtime_error("Call failed: vulkanite::window::Subsystem::queryBackend(): Backend is unrecognised");
    }
}