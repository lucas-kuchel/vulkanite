#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include "configuration.hpp"

#include <cstdint>
#include <span>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace vulkanite::renderer {
    class Device;
    class Buffer;
    class Image;
    class CommandPool;
    class CommandBuffer;
    class CommandBufferCapturePeriod;
    class PipelineLayout;
    class Pipeline;
    class DescriptorSet;

    struct ImageMemoryBarrier;
    struct BufferImageCopyRegion;
    struct BufferCopyRegion;
    struct RenderPassBeginInfo;
    struct Scissor;
    struct Viewport;

    class CommandBuffer {
    public:
        void reset();
        bool beginCapture();
        void beginRenderPass(RenderPassBeginInfo& beginInfo);
        bool endCapture();
        void endRenderPass();
        void copyBuffer(Buffer& source, Buffer& destination, const std::vector<BufferCopyRegion>& copyRegions);
        void copyBufferToImage(Buffer& source, Image& destination, ImageLayout imageLayout, const std::vector<BufferImageCopyRegion>& copyRegions);
        void nextSubpass();
        void pipelineBarrier(Flags sourcePipelineStage, Flags destinationPipelineStage, const std::vector<ImageMemoryBarrier>& memoryBarriers);
        void bindDescriptorSets(DeviceOperation operation, PipelineLayout& layout, std::uint32_t firstSet, const std::vector<DescriptorSet>& sets);
        void bindPipeline(Pipeline& pipeline);
        void bindVertexBuffers(const std::vector<Buffer>& buffers, const std::vector<std::uint64_t>& offsets, std::uint32_t first);
        void bindIndexBuffer(Buffer& buffer, std::uint64_t offset, IndexType indexType);
        void setPipelineViewports(const std::vector<renderer::Viewport>& viewports, std::uint32_t offset);
        void setPipelineScissors(const std::vector<renderer::Scissor>& scissors, std::uint32_t offset);
        void setPipelineLineWidth(float width);
        void setPipelineDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
        void setPipelineBlendConstants(const glm::fvec4& blend);
        void setPipelineDepthBounds(float min, float max);
        void setPipelineStencilCompareMask(Flags faceFlags, std::uint32_t compareMask);
        void setPipelineStencilWriteMask(Flags faceFlags, std::uint32_t writeMask);
        void setPipelineStencilReferenceMask(Flags faceFlags, std::uint32_t reference);
        void pushConstants(PipelineLayout& layout, std::uint32_t stageFlags, std::span<std::uint8_t> data, std::uint32_t offset);
        void draw(std::uint32_t vertexCount, std::uint32_t instances, std::uint32_t firstVertex, std::uint32_t firstInstance);
        void drawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex, std::uint32_t firstInstance, std::int32_t vertexOffset);
        bool capturing();
        bool rendering();

    private:
        CommandBuffer() = default;

        VkCommandBuffer commandBuffer_ = nullptr;
        CommandPool* commandPool_ = nullptr;

        bool capturing_ = false;
        bool rendering_ = false;

        friend class CommandPool;
        friend class Queue;
    };
}

#include "detail/command_buffer.inl"

#endif