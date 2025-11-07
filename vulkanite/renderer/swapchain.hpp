#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <cstdint>
#include <span>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Instance;
    class Surface;
    class Device;
    class Queue;
    class Semaphore;
    class ImageView;
    class Image;
    class Swapchain;

    enum class ImageFormat : int;

    struct SwapchainCreateInfo {
        Surface& surface;
        Device& device;
        Queue& presentQueue;
        Swapchain* oldSwapchain = nullptr;

        std::uint32_t requestedImageCount = 0;
        bool shouldSynchronise = true;
    };

    enum class SwapchainResult {
        SUCCESS,
        BUSY,
        PENDING,
        FAILED,
    };

    class Swapchain {
    public:
        SwapchainResult create(const SwapchainCreateInfo& createInfo);
        void destroy();

        ImageFormat getFormat() const;
        std::uint32_t getImageCount() const;
        std::uint32_t getImageIndex() const;
        std::span<const Image> getImages() const;
        std::span<const ImageView> getImageViews() const;
        bool isSynchronised() const;
        bool shouldRecreate() const;
        glm::uvec2 getExtent() const;

        bool acquireNextImage(Semaphore& acquireSemaphore);
        bool presentNextImage(Semaphore& presentSemaphore);

        bool operator==(const Swapchain& other) const {
            return swapchain_ == other.swapchain_;
        }

        bool operator!=(const Swapchain& other) const {
            return *this != other;
        }

        explicit operator bool() const {
            return swapchain_ && instance_ && surface_ && device_ && presentQueue_;
        }

    private:
        VkSwapchainKHR swapchain_ = nullptr;
        Instance* instance_ = nullptr;
        Surface* surface_ = nullptr;
        Device* device_ = nullptr;
        Queue* presentQueue_ = nullptr;

        VkPresentModeKHR presentMode_;
        VkSurfaceFormatKHR surfaceFormat_;
        VkExtent2D extent_;

        std::uint32_t imageCount_ = 0;
        std::uint32_t imageIndex_ = 0;

        std::vector<Image> images_;
        std::vector<ImageView> imageViews_;

        bool synchronise_ = false;
        bool recreate_ = false;

        VkSurfaceCapabilitiesKHR getSurfaceCapabilities();

        void createImageResources();
        void selectSurfaceFormat();
        void selectPresentMode();
    };
}

#include "detail/swapchain.inl"

#endif