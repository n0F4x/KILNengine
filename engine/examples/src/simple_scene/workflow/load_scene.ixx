module;

#include <filesystem>
#include <memory_resource>

export module examples.simple_scene.workflow.load_scene;

import vulkan_hpp;

import kiln.gfx.asset.gltf.Parser;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.stream.StagingStream;

import examples.simple_scene.workflow.Scene;

namespace demo {

export [[nodiscard]]
auto load_scene(
    const std::filesystem::path&        model_path,
    const kiln::gfx::renderer::Device&  device,
    kiln::gfx::renderer::Allocator&     gpu_allocator,
    kiln::gfx::asset::gltf::Parser&     model_parser,
    kiln::gfx::renderer::StagingStream& staging_stream,
    std::pmr::memory_resource&          transient_memory_resource
) -> Scene;

}   // namespace demo
