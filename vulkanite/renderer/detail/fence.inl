#pragma once

#include "../configuration.hpp"
#include "../device.hpp"
#include "../fence.hpp"

vulkanite::renderer::Fence::Fence(const FenceCreateInfo& createInfo) {
    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = FenceCreateFlags::mapFrom(createInfo.createFlags),
    };

    if (vkCreateFence(createInfo.device.device_, &fenceCreateInfo, nullptr, &fence_) != VK_SUCCESS) {
        fence_ = nullptr;
    }
    else {
        device_ = &createInfo.device;
    }
}

vulkanite::renderer::Fence::~Fence() {
    if (fence_) {
        vkDestroyFence(device_->device_, fence_, nullptr);

        fence_ = nullptr;
    }
}

bool vulkanite::renderer::Fence::signalled() {
    return vkGetFenceStatus(device_->device_, fence_) == VK_SUCCESS;
}