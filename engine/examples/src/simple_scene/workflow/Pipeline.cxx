module;

#include <array>
#include <filesystem>
#include <source_location>

module examples.simple_scene.workflow.Pipeline;

import vulkan_hpp;

import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.vulkan.result.check_result;

namespace demo {

struct ShaderScene {
    vk::DeviceSize primitives;
    vk::DeviceSize materials;
    vk::DeviceSize indices;
    vk::DeviceSize positions;
    vk::DeviceSize vertices;
};

[[nodiscard]]
auto create_graphics_pipeline_layout(const vk::raii::Device& device)
    -> vk::raii::PipelineLayout
{
    constexpr static std::array push_constant_ranges{
        vk::PushConstantRange{ .stageFlags = vk::ShaderStageFlagBits::eVertex,
                              .size       = sizeof(ShaderScene) },
    };

    constexpr static vk::PipelineLayoutCreateInfo create_info{
        .pushConstantRangeCount = push_constant_ranges.size(),
        .pPushConstantRanges    = push_constant_ranges.data(),
    };

    return kiln::gfx::vulkan::check_result(device.createPipelineLayout(create_info));
}

Pipeline::Pipeline(
    const kiln::gfx::renderer::Device&        device,
    const kiln::gfx::renderer::RenderSurface& surface,
    const uint8_t
)
    : m_graphics_pipeline_layout{
          create_graphics_pipeline_layout(device.logical_device())
      },
      m_graphics_shader_module{
          *kiln::gfx::renderer::ShaderModule::load_from_file(
              std::filesystem::path{ std::source_location::current().file_name() }
                  .parent_path()
                  .parent_path()
              / "shaders" / "scene.spv"
          )   //
      },
      m_graphics_pipeline{
          device,
          m_graphics_pipeline_layout,
          m_graphics_shader_module,
          m_graphics_shader_module,
          std::array{ surface.surface_format().format },
      }
{
}

}   // namespace demo
