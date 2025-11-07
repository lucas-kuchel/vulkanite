#pragma once

#include "../command_buffer.hpp"
#include "../command_pool.hpp"
#include "../device.hpp"
#include "../queue.hpp"

void vulkanite::renderer::CommandPool::create(const CommandPoolCreateInfo& createInfo) {
    auto& device = createInfo.device.device_;

    VkCommandPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = createInfo.queue.familyIndex_,
    };

    if (vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool_) != VK_SUCCESS) {
        commandPool_ = nullptr;
    }
    else {
        device_ = &createInfo.device;
        queue_ = &createInfo.queue;
    }
}

void vulkanite::renderer::CommandPool::destroy() {
    if (commandPool_) {
        vkDestroyCommandPool(device_->device_, commandPool_, nullptr);

        commandPool_ = nullptr;
    }
}

std::vector<vulkanite::renderer::CommandBuffer> vulkanite::renderer::CommandPool::allocateCommandBuffers(std::uint32_t count) {
    VkCommandBufferAllocateInfo bufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool_,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<std::uint32_t>(count),
    };

    std::vector<VkCommandBuffer> commandBuffers(count);
    std::vector<CommandBuffer> buffers;

    buffers.reserve(count);

    if (vkAllocateCommandBuffers(device_->device_, &bufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
        return {};
    }

    for (std::uint64_t i = 0; i < count; i++) {
        buffers.push_back(CommandBuffer());

        auto& commandBuffer = buffers.back();

        commandBuffer.commandPool_ = this;

        buffers[i].commandBuffer_ = commandBuffers[i];
        buffers[i].capturing_ = false;
    }

    return buffers;
}

void vulkanite::renderer::CommandPool::destroyCommandBuffers(const std::vector<CommandBuffer>& buffers) {
    std::vector<VkCommandBuffer> commandBuffers(buffers.size());

    for (std::uint64_t i = 0; i < buffers.size(); i++) {
        commandBuffers[i] = buffers[i].commandBuffer_;
    }

    vkFreeCommandBuffers(device_->device_, commandPool_, static_cast<std::uint32_t>(commandBuffers.size()), commandBuffers.data());
}

bool vulkanite::renderer::CommandPool::resetAllCommandBuffers() {
    return vkResetCommandPool(device_->device_, commandPool_, 0) == VK_SUCCESS;
}