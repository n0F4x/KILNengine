module;

#include <filesystem>
#include <memory_resource>
#include <span>

export module examples.frustum_culling.workflow.load_scene;

import vulkan_hpp;

import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.stream.StagingStream;

import examples.frustum_culling.workflow.ModelDescription;
import examples.frustum_culling.workflow.Scene;

namespace demo {

export [[nodiscard]]
auto load_scene(
    std::span<const ModelDescription>   model_descriptions,
    const kiln::gfx::renderer::Device&  device,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    bool                                disable_culling,
    std::pmr::memory_resource&          transient_memory_resource
) -> Scene;

}   // namespace demo
