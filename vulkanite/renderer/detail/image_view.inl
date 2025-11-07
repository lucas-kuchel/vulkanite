#pragma once

#include "../device.hpp"
#include "../image.hpp"
#include "../image_view.hpp"

vulkanite::renderer::ImageView::ImageView(const ImageViewCreateInfo& createInfo) {
    VkImageViewCreateInfo viewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = createInfo.image.image_,
        .viewType = ImageView::mapType(createInfo.type),
        .format = createInfo.image.format_,
        .components = {},
        .subresourceRange = {
            .aspectMask = ImageAspectFlags::mapFrom(createInfo.aspectFlags),
            .baseMipLevel = createInfo.baseMipLevel,
            .levelCount = createInfo.levelCount,
            .baseArrayLayer = createInfo.baseArrayLayer,
            .layerCount = createInfo.layerCount,
        },
    };

    if (vkCreateImageView(createInfo.image.device_->device_, &viewCreateInfo, nullptr, &imageView_) != VK_SUCCESS) {
        imageView_ = nullptr;
    }
    else {
        imageViewType_ = mapType(createInfo.type);
        image_ = createInfo.image.image_;
        baseArrayLayer_ = createInfo.baseArrayLayer;
        baseMipLevel_ = createInfo.baseMipLevel;
        layerCount_ = createInfo.layerCount;
        levelCount_ = createInfo.levelCount;
        extent_ = createInfo.image.extent_;
        device_ = createInfo.image.device_->device_;
    }
}

vulkanite::renderer::ImageView::~ImageView() {
    if (imageView_) {
        vkDestroyImageView(device_, imageView_, nullptr);

        imageView_ = nullptr;
    }
}

vulkanite::renderer::ImageViewType vulkanite::renderer::ImageView::getType() const {
    return reverseMapType(imageViewType_);
}

std::uint32_t vulkanite::renderer::ImageView::getBaseMipLevel() const {
    return baseMipLevel_;
}

std::uint32_t vulkanite::renderer::ImageView::getLevelCount() const {
    return levelCount_;
}

std::uint32_t vulkanite::renderer::ImageView::getBaseArrayLayer() const {
    return baseArrayLayer_;
}

std::uint32_t vulkanite::renderer::ImageView::getLayerCount() const {
    return layerCount_;
}

VkImageViewType vulkanite::renderer::ImageView::mapType(ImageViewType type) {
    switch (type) {
        case ImageViewType::IMAGE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;

        case ImageViewType::IMAGE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;

        case ImageViewType::IMAGE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;

        default:
            return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
}

vulkanite::renderer::ImageViewType vulkanite::renderer::ImageView::reverseMapType(VkImageViewType type) {
    switch (type) {
        case VK_IMAGE_VIEW_TYPE_1D:
            return ImageViewType::IMAGE_1D;

        case VK_IMAGE_VIEW_TYPE_2D:
            return ImageViewType::IMAGE_2D;

        case VK_IMAGE_VIEW_TYPE_3D:
            return ImageViewType::IMAGE_3D;

        default:
            return ImageViewType::IMAGE_1D;
    }
}