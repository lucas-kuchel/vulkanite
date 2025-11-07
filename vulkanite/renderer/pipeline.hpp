#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include "configuration.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class ShaderModule;
    class Device;
    class RenderPass;
    class Buffer;
    class ImageView;
    class Sampler;

    struct ShaderStageInfo {
        ShaderModule& module;
        ShaderStage stage;
    };

    struct VertexInputBindingDescription {
        VertexInputRate inputRate;

        std::uint32_t binding;
        std::uint32_t strideBytes;
    };

    struct VertexAttributeDescription {
        VertexAttributeFormat format;

        std::uint32_t binding;
        std::uint32_t location;
    };

    struct PipelineInputAssemblyState {
        PolygonTopology topology;

        bool primitiveRestart = false;
    };

    struct PipelineVertexInputState {
        std::vector<VertexInputBindingDescription> bindings;
        std::vector<VertexAttributeDescription> attributes;
    };

    struct Scissor {
        glm::ivec2 offset;
        glm::uvec2 extent;
    };

    struct Viewport {
        glm::fvec2 position = {};
        glm::fvec2 extent = {};

        float minDepth = 0.0f;
        float maxDepth = 1.0f;
    };

    struct PerFaceRasterisationState {
        CompareOperation depthComparison;
        CompareOperation stencilComparison;
        ValueOperation stencilFailOperation;
        ValueOperation depthFailOperation;
        ValueOperation passOperation;

        std::uint32_t stencilCompareMask = 0xF;
        std::uint32_t stencilWriteMask = 0xF;
    };

    struct RasterisationState {
        PolygonFaceWinding frontFaceWinding;
        PolygonCullMode cullMode;
        PerFaceRasterisationState frontface = {};
        PerFaceRasterisationState backface = {};

        bool depthClampEnable = false;
        bool depthTestEnable = false;
        bool depthWriteEnable = false;
        bool depthBoundsTestEnable = false;
        bool stencilTestEnable = false;
    };

    struct MultisampleState {
        std::uint32_t sampleCount = 1;
        bool sampleShadingEnable = false;
        bool alphaToCoverageEnable = false;
        bool alphaToOneEnable = false;
        float minSampleShading = 0.0;
    };

    struct ColourBlendAttachment {
        bool blendEnable = false;

        BlendFactor sourceColourBlendFactor;
        BlendFactor destinationColourBlendFactor;
        BlendOperation colourBlendOperation;
        BlendFactor sourceAlphaBlendFactor;
        BlendFactor destinationAlphaBlendFactor;
        BlendOperation alphaBlendOperation;
    };

    struct ColourBlendState {
        std::vector<ColourBlendAttachment> attachments;
    };

    struct DescriptorSetInputInfo {
        DescriptorInputType type;
        Flags stageFlags;

        std::uint32_t count;
        std::uint32_t binding;
    };

    struct DescriptorSetLayoutCreateInfo {
        Device& device;

        std::vector<DescriptorSetInputInfo> inputs;
    };

    class DescriptorSetLayout {
    public:
        void create(const DescriptorSetLayoutCreateInfo& createInfo);
        void destroy();

    private:
        VkDescriptorSetLayout descriptorSetLayout_ = nullptr;
        Device* device_ = nullptr;

        friend class DescriptorSet;
        friend class DescriptorPool;
        friend class PipelineLayout;
    };

    struct PushConstantInputInfo {
        std::uint32_t sizeBytes;

        Flags stageFlags;
    };

    struct PipelineLayoutCreateInfo {
        Device& device;

        std::vector<DescriptorSetLayout> inputLayouts;
        std::vector<PushConstantInputInfo> pushConstants;
    };

    struct DescriptorSetBufferBinding {
        Buffer& buffer;

        std::uint64_t offsetBytes;
        std::uint64_t rangeBytes;
    };

    struct DescriptorSetImageBinding {
        ImageView& image;
        Sampler& sampler;
        ImageLayout layout;
    };

    struct DescriptorPoolSize {
        DescriptorInputType type;

        std::uint32_t count;
    };

    struct DescriptorPoolCreateInfo {
        Device& device;

        std::vector<DescriptorPoolSize> poolSizes;

        std::uint32_t maximumSetCount;
    };

    class DescriptorPool;

    struct DescriptorSetCreateInfo {
        std::vector<DescriptorSetLayout> layouts;
    };

    class DescriptorSet {
    private:
        VkDescriptorSet descriptorSet_ = nullptr;
        const DescriptorSetLayout* layout_ = nullptr;
        DescriptorPool* pool_ = nullptr;

        friend class DescriptorPool;
        friend class CommandBuffer;
    };

    struct DescriptorSetUpdateInfo {
        DescriptorSet& set;
        DescriptorInputType inputType;

        std::uint32_t binding;
        std::uint32_t arrayElement;

        std::vector<DescriptorSetBufferBinding> buffers;
        std::vector<DescriptorSetImageBinding> images;
    };

    class DescriptorPool {
    public:
        void create(const DescriptorPoolCreateInfo& createInfo);
        void destroy();

        std::vector<DescriptorSet> allocateDescriptorSets(const DescriptorSetCreateInfo& createInfo);
        void updateDescriptorSets(std::vector<DescriptorSetUpdateInfo> updateInfos);

    private:
        VkDescriptorPool descriptorPool_ = nullptr;
        Device* device_ = nullptr;
    };

    class PipelineLayout {
    public:
        void create(const PipelineLayoutCreateInfo& createInfo);
        void destroy();

    private:
        VkPipelineLayout pipelineLayout_ = nullptr;
        Device* device_ = nullptr;

        friend class Device;
        friend class CommandBuffer;
    };

    struct PipelineCreateInfo {
        RenderPass& renderPass;
        PipelineLayout& layout;

        std::vector<ShaderStageInfo> shaderStages;

        std::uint32_t subpassIndex;
        std::uint32_t viewportCount;
        std::uint32_t scissorCount;

        PipelineVertexInputState vertexInput;
        PipelineInputAssemblyState inputAssembly;
        RasterisationState rasterisation;
        MultisampleState multisample;
        ColourBlendState colourBlend;
    };

    class Pipeline {
    public:
        void destroy();

    private:
        VkPipeline pipeline_ = nullptr;
        Device* device_ = nullptr;

        static VkShaderStageFlagBits reverseMapShaderStage(ShaderStage stage);
        static VkVertexInputRate reverseMapVertexInputRate(VertexInputRate rate);
        static VkPrimitiveTopology reverseMapPrimitive(PolygonTopology topology);
        static VkCullModeFlags reverseMapCullMode(PolygonCullMode cullMode);
        static VkFrontFace reverseMapFrontFace(PolygonFaceWinding winding);
        static VkFormat reverseMapVertexAttributeFormat(VertexAttributeFormat format);
        static VkSampleCountFlagBits reverseMapSampleCount(std::uint32_t sampleCount);
        static VkCompareOp reverseMapCompareOperation(CompareOperation compare);
        static VkStencilOpState reverseMapStencilOperationState(PerFaceRasterisationState perFaceState);
        static VkBlendFactor reverseMapBlendFactor(BlendFactor factor);
        static VkBlendOp reverseMapBlendOperation(BlendOperation operation);
        static VkStencilOp reverseMapStencilOperation(ValueOperation operation);

        friend class Device;
        friend class CommandBuffer;
    };
}

#include "detail/pipeline.inl"

#endif