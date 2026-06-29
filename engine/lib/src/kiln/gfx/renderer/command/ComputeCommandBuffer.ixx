module;

#include <cstdint>

export module kiln.gfx.renderer.command.ComputeCommandBuffer;

import vulkan;

import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.pipeline.ComputePipeline;
import kiln.gfx.renderer.memory.BufferRegion;

namespace kiln::gfx::renderer {

export class ComputeCommandBuffer : public TransferCommandBuffer {
public:
    using TransferCommandBuffer::TransferCommandBuffer;

    auto record_buffer_fill(const BufferRegion& buffer_region, uint32_t data) -> void;
    auto record_pipeline_bind(const ComputePipeline& pipeline) -> void;
    auto record_push_constants(const vk::PushConstantsInfo& info) -> void;
    auto record_dispatch(
        uint32_t group_count_x,
        uint32_t group_count_y,
        uint32_t group_count_z
    ) -> void;
};

}   // namespace kiln::gfx::renderer
