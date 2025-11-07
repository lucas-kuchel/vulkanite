#pragma once

#include "../device.hpp"
#include "../image.hpp"
#include "../image_view.hpp"
#include "../swapchain.hpp"

#include "../../window/window.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

vulkanite::renderer::Swapchain::Swapchain(const SwapchainCreateInfo& createInfo, SwapchainResult& result) {
    VkSwapchainKHR oldSwapchain = nullptr;

    synchronise_ = createInfo.shouldSynchronise;
    instance_ = createInfo.device.instance_;
    device_ = &createInfo.device;
    surface_ = &createInfo.surface;
    presentQueue_ = &createInfo.presentQueue;
    recreate_ = false;

    VkSurfaceCapabilitiesKHR surfaceCapabilities = getSurfaceCapabilities();

    if (surfaceCapabilities.currentExtent.width == 0 || surfaceCapabilities.currentExtent.height == 0) {
        result = SwapchainResult::PENDING;

        return;
    }

    selectSurfaceFormat();
    selectPresentMode();

    std::uint32_t imageCountMaximum = std::min(createInfo.requestedImageCount, surfaceCapabilities.maxImageCount);
    imageCount_ = std::max(imageCountMaximum, surfaceCapabilities.minImageCount);

    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
        extent_ = surfaceCapabilities.currentExtent;
    }

    extent_.width = std::clamp(extent_.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
    extent_.height = std::clamp(extent_.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = createInfo.surface.surface_,
        .minImageCount = imageCount_,
        .imageFormat = surfaceFormat_.format,
        .imageColorSpace = surfaceFormat_.colorSpace,
        .imageExtent = extent_,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode_,
        .clipped = VK_TRUE,
        .oldSwapchain = oldSwapchain,
    };

    VkResult error = vkCreateSwapchainKHR(createInfo.device.device_, &swapchainCreateInfo, nullptr, &swapchain_);

    if (error == VK_ERROR_NATIVE_WINDOW_IN_USE_KHR) {
        if (createInfo.oldSwapchain) {
            createInfo.oldSwapchain->~Swapchain();
        }

        swapchainCreateInfo.oldSwapchain = nullptr;

        error = vkCreateSwapchainKHR(createInfo.device.device_, &swapchainCreateInfo, nullptr, &swapchain_);
    }

    if (error == VK_ERROR_NATIVE_WINDOW_IN_USE_KHR) {
        result = SwapchainResult::BUSY;
    }
    else if (error == VK_ERROR_OUT_OF_DATE_KHR) {
        result = SwapchainResult::PENDING;
    }
    else if (error != VK_SUCCESS) {
        result = SwapchainResult::FAILED;
    }
    else {
        if (createInfo.oldSwapchain) {
            createInfo.oldSwapchain->~Swapchain();
        }

        createImageResources();

        result = SwapchainResult::SUCCESS;
    }
}

vulkanite::renderer::Swapchain::~Swapchain() {
    if (swapchain_) {
        for (auto& image : images_) {
            image.image_ = nullptr;
        }

        images_.clear();
        imageViews_.clear();

        vkDestroySwapchainKHR(device_->device_, swapchain_, nullptr);

        swapchain_ = nullptr;
    }
}

bool vulkanite::renderer::Swapchain::acquireNextImage(Semaphore& acquireSemaphore) {
    if (recreate_) {
        return false;
    }

    VkResult result = vkAcquireNextImageKHR(device_->device_, swapchain_, UINT32_MAX, acquireSemaphore.semaphore_, nullptr, &imageIndex_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_ = true;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        return false;
    }

    return true;
}

bool vulkanite::renderer::Swapchain::presentNextImage(Semaphore& presentSemaphore) {
    auto& queue = presentQueue_->queue_;

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &presentSemaphore.semaphore_,
        .swapchainCount = 1,
        .pSwapchains = &swapchain_,
        .pImageIndices = &imageIndex_,
        .pResults = nullptr,
    };

    VkResult result = vkQueuePresentKHR(queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreate_ = true;
    }
    else if (result != VK_SUCCESS) {
        return false;
    }

    return true;
}

vulkanite::renderer::ImageFormat vulkanite::renderer::Swapchain::getFormat() const {
    return Image::reverseMapFormat(images_.front().format_);
}

std::uint32_t vulkanite::renderer::Swapchain::getImageCount() const {
    return imageCount_;
}

std::uint32_t vulkanite::renderer::Swapchain::getImageIndex() const {
    return imageIndex_;
}

std::span<const vulkanite::renderer::Image> vulkanite::renderer::Swapchain::getImages() const {
    return images_;
}

std::span<const vulkanite::renderer::ImageView> vulkanite::renderer::Swapchain::getImageViews() const {
    return imageViews_;
}

bool vulkanite::renderer::Swapchain::isSynchronised() const {
    return synchronise_;
}

bool vulkanite::renderer::Swapchain::shouldRecreate() const {
    return recreate_;
}

glm::uvec2 vulkanite::renderer::Swapchain::getExtent() const {
    return {extent_.width, extent_.height};
}

void vulkanite::renderer::Swapchain::createImageResources() {
    auto& device = device_->device_;

    std::uint32_t actualImageCount = 0;

    if (vkGetSwapchainImagesKHR(device, swapchain_, &actualImageCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Construction failed: renderer::Swapchain: Failed to query swapchain images");
    }

    std::vector<VkImage> queriedImages(actualImageCount);

    if (vkGetSwapchainImagesKHR(device, swapchain_, &actualImageCount, queriedImages.data()) != VK_SUCCESS) {
        throw std::runtime_error("Construction failed: renderer::Swapchain: Failed to query swapchain images");
    }

    imageCount_ = actualImageCount;

    images_.reserve(imageCount_);
    imageViews_.reserve(imageCount_);

    for (std::uint64_t i = 0; i < imageCount_; i++) {
        images_.push_back(Image());

        Image& image = images_.back();

        image.isHostCoherent_ = false;
        image.isHostVisible_ = false;
        image.image_ = queriedImages[i];
        image.extent_ = {extent_.width, extent_.height, 1};
        image.format_ = surfaceFormat_.format;
        image.type_ = VK_IMAGE_TYPE_2D;
        image.arrayLayers_ = 1;
        image.mipLevels_ = 1;
        image.sampleCount_ = 1;
        image.device_ = device_;

        ImageViewCreateInfo viewCreateInfo = {
            .image = image,
            .type = ImageViewType::IMAGE_2D,
            .aspectFlags = ImageAspectFlags::COLOUR,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        imageViews_.emplace_back(viewCreateInfo);
    }
}

VkSurfaceCapabilitiesKHR vulkanite::renderer::Swapchain::getSurfaceCapabilities() {
    auto& physicalDevice = instance_->physicalDevice_;
    auto& surface = surface_->surface_;
    auto extent = surface_->getExtent();

    VkSurfaceCapabilitiesKHR surfaceCapabilities;

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities) != VK_SUCCESS) {
        extent_ = {
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
        };

        return {};
    }

    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
        extent_ = surfaceCapabilities.currentExtent;
    }
    else {
        auto& minExtent = surfaceCapabilities.minImageExtent;
        auto& maxExtent = surfaceCapabilities.maxImageExtent;

        extent_.width = std::clamp(extent.x, minExtent.width, maxExtent.width);
        extent_.height = std::clamp(extent.y, minExtent.height, maxExtent.height);
    }

    return surfaceCapabilities;
}

void vulkanite::renderer::Swapchain::selectSurfaceFormat() {
    auto& physicalDevice = instance_->physicalDevice_;
    auto& surface = surface_->surface_;

    std::uint32_t formatCount = 0;

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Call failed: renderer::Swapchain::create(): Failed to get surface formats");
    }

    std::vector<VkSurfaceFormatKHR> formats(formatCount);

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data()) != VK_SUCCESS) {
        throw std::runtime_error("Call failed: renderer::Swapchain::create(): Failed to get surface formats");
    }

    surfaceFormat_ = formats[0];

    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        surfaceFormat_.format = VK_FORMAT_B8G8R8A8_SRGB;
        surfaceFormat_.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
    else {
        for (const auto& format : formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat_ = format;
                break;
            }
        }
    }
}

void vulkanite::renderer::Swapchain::selectPresentMode() {
    auto& physicalDevice = instance_->physicalDevice_;
    auto& surface = surface_->surface_;

    std::uint32_t presentModeCount = 0;

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr) != VK_SUCCESS) {
        presentMode_ = VK_PRESENT_MODE_MAX_ENUM_KHR;

        return;
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()) != VK_SUCCESS) {
        presentMode_ = VK_PRESENT_MODE_MAX_ENUM_KHR;

        return;
    }

    presentMode_ = VK_PRESENT_MODE_FIFO_KHR;

    std::int32_t chosenPriority = -1;

    for (const auto& mode : presentModes) {
        std::int32_t priority = -1;

        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            priority = 3;
        }
        else if (!synchronise_ && mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            priority = 2;
        }
        else if (mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
            priority = 1;
        }
        else if (mode == VK_PRESENT_MODE_FIFO_KHR) {
            priority = 0;
        }

        if (priority > chosenPriority) {
            chosenPriority = priority;
            presentMode_ = mode;
        }
    }
}