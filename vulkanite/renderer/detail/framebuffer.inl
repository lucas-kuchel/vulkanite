#pragma once

#include "../device.hpp"
#include "../framebuffer.hpp"
#include "../image.hpp"
#include "../image_view.hpp"
#include "../render_pass.hpp"

inline void vulkanite::renderer::Framebuffer::create(const FramebufferCreateInfo& createInfo) {
    std::vector<VkImageView> imageViews;

    imageViews.reserve(createInfo.imageViews.size());

    for (auto& imageView : createInfo.imageViews) {
        imageViews.push_back(imageView.imageView_);
    }

    auto& defactoImage = createInfo.imageViews.front();

    VkFramebufferCreateInfo framebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = createInfo.renderPass.renderPass_,
        .attachmentCount = static_cast<std::uint32_t>(createInfo.imageViews.size()),
        .pAttachments = imageViews.data(),
        .width = defactoImage.extent_.x,
        .height = defactoImage.extent_.y,
        .layers = 1,
    };

    if (vkCreateFramebuffer(createInfo.device.device_, &framebufferCreateInfo, nullptr, &framebuffer_) != VK_SUCCESS) {
        framebuffer_ = nullptr;
    }
    else {
        device_ = &createInfo.device;
    }
}

inline void vulkanite::renderer::Framebuffer::destroy() {
    if (framebuffer_) {
        vkDestroyFramebuffer(device_->device_, framebuffer_, nullptr);

        framebuffer_ = nullptr;
    }
}