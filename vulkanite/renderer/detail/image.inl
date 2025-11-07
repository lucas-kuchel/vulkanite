#pragma once

#include "../device.hpp"
#include "../image.hpp"
#include "../instance.hpp"

#include <stdexcept>

void vulkanite::renderer::Image::create(const ImageCreateInfo& createInfo) {
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

    VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = mapType(createInfo.type),
        .format = mapFormat(createInfo.format, *createInfo.device.instance_),
        .extent = {createInfo.extent.x, createInfo.extent.y, createInfo.extent.z},
        .mipLevels = createInfo.mipLevels,
        .arrayLayers = createInfo.arrayLayers,
        .samples = static_cast<VkSampleCountFlagBits>(createInfo.sampleCount),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = ImageUsageFlags::mapFrom(createInfo.usageFlags),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationInfo allocationInfo = {};

    if (vmaCreateImage(createInfo.device.allocator_, &imageCreateInfo, &allocationCreateInfo, &image_, &allocation_, &allocationInfo) != VK_SUCCESS) {
        image_ = nullptr;
        allocation_ = nullptr;
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
        arrayLayers_ = createInfo.arrayLayers;
        mipLevels_ = createInfo.mipLevels;
        sampleCount_ = createInfo.sampleCount;
        type_ = imageCreateInfo.imageType;
        extent_ = createInfo.extent;
        format_ = imageCreateInfo.format;
    }
}

void vulkanite::renderer::Image::destroy() {
    if (image_) {
        vmaDestroyImage(device_->allocator_, image_, allocation_);
    }
}

vulkanite::renderer::ImageMapping vulkanite::renderer::Image::map(std::uint64_t sizeBytes, std::uint64_t offsetBytes) {
    ImageMapping mapping;

    mapping.offset = offsetBytes;

    if (!isHostCoherent_) {
        auto& instance = device_->instance_;
        auto& properties = instance->properties_;

        VkDeviceSize atomSize = properties.limits.nonCoherentAtomSize;

        mapping.alignedOffset = offsetBytes & ~(atomSize - 1);
        mapping.alignedSize = ((mapping.alignedOffset + sizeBytes + atomSize - 1) & ~(atomSize - 1)) - mapping.alignedOffset;

        vmaInvalidateAllocation(device_->allocator_, allocation_, mapping.alignedOffset, mapping.alignedSize);
    }
    else {
        mapping.alignedOffset = offsetBytes;
        mapping.alignedSize = sizeBytes;
    }

    void* data = nullptr;

    vmaMapMemory(device_->allocator_, allocation_, &data);

    mapping.data = {reinterpret_cast<std::uint8_t*>(data) + offsetBytes, sizeBytes};

    return mapping;
}

void vulkanite::renderer::Image::unmap(ImageMapping& mapping) {
    if (!isHostCoherent_) {
        vmaFlushAllocation(device_->allocator_, allocation_, mapping.alignedOffset, mapping.alignedSize);
    }

    vmaUnmapMemory(device_->allocator_, allocation_);
}

bool vulkanite::renderer::Image::canBeMapped() const {
    return isHostCoherent_;
}

vulkanite::renderer::ImageFormat vulkanite::renderer::Image::getFormat() const {
    return reverseMapFormat(format_);
}

vulkanite::renderer::ImageType vulkanite::renderer::Image::getType() const {
    return reverseMapType(type_);
}

std::uint64_t vulkanite::renderer::Image::getSize() const {
    return size_;
}

glm::uvec3 vulkanite::renderer::Image::getExtent() const {
    return extent_;
}

std::uint32_t vulkanite::renderer::Image::getSampleCount() const {
    return sampleCount_;
}

std::uint32_t vulkanite::renderer::Image::getMipLevels() const {
    return mipLevels_;
}

std::uint32_t vulkanite::renderer::Image::getArrayLayers() const {
    return arrayLayers_;
}

VkFormat vulkanite::renderer::Image::mapFormat(ImageFormat format, Instance& instance) {
    auto findSupportedFormat = [&](const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) -> VkFormat {
        for (VkFormat vkFormat : candidates) {
            VkFormatProperties properties;

            vkGetPhysicalDeviceFormatProperties(instance.physicalDevice_, vkFormat, &properties);

            if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
                return vkFormat;
            }
            if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
                return vkFormat;
            }
        }

        throw std::runtime_error("Call failed: renderer::Image::mapFormat(): Failed to find supported non-colour image format");
    };

    switch (format) {
        case ImageFormat::R8_UNORM:
            return VK_FORMAT_R8_UNORM;

        case ImageFormat::R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;

        case ImageFormat::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;

        case ImageFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case ImageFormat::B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;

        case ImageFormat::R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        case ImageFormat::R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case ImageFormat::B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;

        case ImageFormat::DEPTH_ONLY: {
            std::vector<VkFormat> candidates = {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D16_UNORM,
                VK_FORMAT_D16_UNORM_S8_UINT,
            };

            return findSupportedFormat(candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        case ImageFormat::STENCIL_ONLY: {
            std::vector<VkFormat> candidates = {
                VK_FORMAT_S8_UINT,
            };

            return findSupportedFormat(candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        case ImageFormat::DEPTH_STENCIL: {
            std::vector<VkFormat> candidates = {
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D16_UNORM_S8_UINT,
            };

            return findSupportedFormat(candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        default:
            return VK_FORMAT_MAX_ENUM;
    }
}

VkImageType vulkanite::renderer::Image::mapType(ImageType type) {
    switch (type) {
        case ImageType::IMAGE_1D:
            return VK_IMAGE_TYPE_1D;

        case ImageType::IMAGE_2D:
            return VK_IMAGE_TYPE_2D;

        case ImageType::IMAGE_3D:
            return VK_IMAGE_TYPE_3D;

        default:
            return VK_IMAGE_TYPE_MAX_ENUM;
    }
}

vulkanite::renderer::ImageFormat vulkanite::renderer::Image::reverseMapFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R8_UNORM:
            return ImageFormat::R8_UNORM;

        case VK_FORMAT_R8G8_UNORM:
            return ImageFormat::R8G8_UNORM;

        case VK_FORMAT_R8G8B8_UNORM:
            return ImageFormat::R8G8B8_UNORM;

        case VK_FORMAT_R8G8B8A8_UNORM:
            return ImageFormat::R8G8B8A8_UNORM;

        case VK_FORMAT_B8G8R8A8_UNORM:
            return ImageFormat::B8G8R8A8_UNORM;

        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return ImageFormat::R16G16B16A16_SFLOAT;

        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return ImageFormat::R32G32B32A32_SFLOAT;

        case VK_FORMAT_B8G8R8A8_SRGB:
            return ImageFormat::B8G8R8A8_SRGB;

        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D16_UNORM:
            return ImageFormat::DEPTH_ONLY;

        case VK_FORMAT_S8_UINT:
            return ImageFormat::STENCIL_ONLY;

        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return ImageFormat::DEPTH_STENCIL;

        default:
            return ImageFormat::B8G8R8A8_SRGB;
    }
}

vulkanite::renderer::ImageType vulkanite::renderer::Image::reverseMapType(VkImageType type) {
    switch (type) {
        case VK_IMAGE_TYPE_1D:
            return ImageType::IMAGE_1D;

        case VK_IMAGE_TYPE_2D:
            return ImageType::IMAGE_2D;

        case VK_IMAGE_TYPE_3D:
            return ImageType::IMAGE_3D;

        default:
            return ImageType::IMAGE_1D;
    }
}