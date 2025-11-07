#pragma once

#include "../configuration.hpp"
#include "../device.hpp"
#include "../fence.hpp"

vulkanite::renderer::Semaphore::Semaphore(Device& device) {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    if (vkCreateSemaphore(device.device_, &semaphoreCreateInfo, nullptr, &semaphore_) != VK_SUCCESS) {
        semaphore_ = nullptr;
    }
    else {
        device_ = &device;
    }
}

vulkanite::renderer::Semaphore::~Semaphore() {
    if (semaphore_ != nullptr) {
        vkDestroySemaphore(device_->device_, semaphore_, nullptr);

        semaphore_ = nullptr;
    }
}