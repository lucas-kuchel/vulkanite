#pragma once

#include "../buffer.hpp"
#include "../device.hpp"
#include "../instance.hpp"

vulkanite::renderer::Buffer::Buffer(const BufferCreateInfo& createInfo) {
    VmaMemoryUsage memoryUsage;
    VkMemoryPropertyFlags memoryProperties;

    switch (createInfo.memoryType) {
        case MemoryType::DEVICE_LOCAL:
            memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
            memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;

        case MemoryType::HOST_VISIBLE:
            memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
    }

    VmaAllocationCreateInfo allocationCreateInfo = {
        .flags = 0,
        .usage = memoryUsage,
        .requiredFlags = 0,
        .preferredFlags = memoryProperties,
        .memoryTypeBits = 0,
        .pool = nullptr,
        .pUserData = nullptr,
        .priority = 0.0,
    };

    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = createInfo.sizeBytes,
        .usage = BufferUsageFlags::mapFrom(createInfo.usageFlags),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    VmaAllocationInfo allocationInfo = {};

    if (vmaCreateBuffer(createInfo.device.allocator_, &bufferCreateInfo, &allocationCreateInfo, &buffer_, &allocation_, &allocationInfo) != VK_SUCCESS) {
        allocation_ = nullptr;
        buffer_ = nullptr;
    }
    else {
        device_ = &createInfo.device;

        auto& instance = device_->instance_;
        auto& properties = instance->memoryProperties_;

        VkMemoryPropertyFlags flags = properties.memoryTypes[allocationInfo.memoryType].propertyFlags;

        isHostCoherent_ = (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
        isHostVisible_ = (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
        device_ = &createInfo.device;
        size_ = allocationInfo.size;
    }
}

vulkanite::renderer::Buffer::~Buffer() {
    if (buffer_) {
        vmaDestroyBuffer(device_->allocator_, buffer_, allocation_);

        buffer_ = nullptr;
    }
}

vulkanite::renderer::BufferMapping vulkanite::renderer::Buffer::map(std::uint64_t size, std::uint64_t offset) {
    BufferMapping mapping;

    mapping.offset = offset;

    if (!isHostCoherent_) {
        auto& instance = device_->instance_;
        auto& properties = instance->properties_;

        VkDeviceSize atomSize = properties.limits.nonCoherentAtomSize;

        mapping.alignedOffset = offset & ~(atomSize - 1);
        mapping.alignedSize = ((mapping.alignedOffset + size + atomSize - 1) & ~(atomSize - 1)) - mapping.alignedOffset;

        vmaInvalidateAllocation(device_->allocator_, allocation_, mapping.alignedOffset, mapping.alignedSize);
    }
    else {
        mapping.alignedOffset = offset;
        mapping.alignedSize = size;
    }

    void* data = nullptr;

    vmaMapMemory(device_->allocator_, allocation_, &data);

    mapping.data = {reinterpret_cast<std::uint8_t*>(data) + offset, size};

    return mapping;
}

void vulkanite::renderer::Buffer::unmap(BufferMapping& mapping) {
    if (!isHostCoherent_) {
        vmaFlushAllocation(device_->allocator_, allocation_, mapping.alignedOffset, mapping.alignedSize);
    }

    vmaUnmapMemory(device_->allocator_, allocation_);
}

std::uint64_t vulkanite::renderer::Buffer::getSize() const {
    return size_;
}

bool vulkanite::renderer::Buffer::canBeMapped() const {
    return isHostVisible_;
}