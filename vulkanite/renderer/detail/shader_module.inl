#pragma once

#include "../device.hpp"
#include "../shader_module.hpp"

inline void vulkanite::renderer::ShaderModule::create(const ShaderModuleCreateInfo& createInfo) {
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = static_cast<std::uint32_t>(createInfo.data.size() * sizeof(std::uint32_t)),
        .pCode = createInfo.data.data(),
    };

    if (vkCreateShaderModule(createInfo.device.device_, &shaderModuleCreateInfo, nullptr, &module_) != VK_SUCCESS) {
        module_ = nullptr;
    }
    else {
        device_ = &createInfo.device;
    }
}

inline void vulkanite::renderer::ShaderModule::destroy() {
    if (module_) {
        vkDestroyShaderModule(device_->device_, module_, nullptr);
    }
}