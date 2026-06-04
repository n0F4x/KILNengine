module;

#include <cstdint>
#include <filesystem>
#include <memory_resource>

export module examples.simple_scene.load_scene;

import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.stream.StagingStream;

import examples.simple_scene.workflow.Scene;

namespace demo {

export [[nodiscard]]
auto load_scene(
    const kiln::gfx::renderer::Device&  device,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::renderer::StagingStream& staging_stream,
    kiln::gfx::asset::gltf::Parser&     model_parser,
    const std::filesystem::path&        model_filepath,
    bool                                disable_culling,
    uint32_t                            grid_size,
    std::pmr::memory_resource&          transient_memory_resource
) -> Scene;

}   // namespace demo
