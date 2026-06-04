module;

#include <cstdint>

module kiln.gfx.renderer.command.ComputeCommandBuffer;

import vulkan_hpp;

namespace kiln::gfx::renderer {

// ReSharper disable once CppMemberFunctionMayBeConst
auto ComputeCommandBuffer::record_buffer_fill(
    const BufferRegion& buffer_region,
    const uint32_t      data
) -> void
{
    get().fillBuffer(
        buffer_region.buffer().get(),
        buffer_region.offset(),
        buffer_region.size(),
        data
    );
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto ComputeCommandBuffer::record_pipeline_bind(const ComputePipeline& pipeline) -> void
{
    get().bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.get());
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto ComputeCommandBuffer::record_push_constants(const vk::PushConstantsInfo& info)
    -> void
{
    get().pushConstants2(info);
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto ComputeCommandBuffer::record_dispatch(
    const uint32_t group_count_x,
    const uint32_t group_count_y,
    const uint32_t group_count_z
) -> void
{
    get().dispatch(group_count_x, group_count_y, group_count_z);
}

}   // namespace kiln::gfx::renderer
