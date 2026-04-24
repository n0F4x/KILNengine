module;

#include <array>
#include <filesystem>
#include <source_location>

module examples.simple_scene.workflow.Renderer;

import vulkan_hpp;

import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.vulkan.result.check_result;

import examples.simple_scene.Window;

namespace demo {

[[nodiscard]]
auto create_graphics_pipeline_layout(const vk::raii::Device& device)
    -> vk::raii::PipelineLayout
{
    constexpr static std::array push_constant_ranges{
        vk::PushConstantRange{ .stageFlags = vk::ShaderStageFlagBits::eVertex,
                              .size       = sizeof(vk::DeviceSize) },
    };

    constexpr static vk::PipelineLayoutCreateInfo create_info{
        .pushConstantRangeCount = push_constant_ranges.size(),
        .pPushConstantRanges    = push_constant_ranges.data(),
    };

    return kiln::gfx::vulkan::check_result(device.createPipelineLayout(create_info));
}

Renderer::Renderer(
    const kiln::gfx::vulkan::Instance& vulkan_instance,
    const kiln::gfx::renderer::Device& device,
    Window&                            window
)
    : m_surface{
          kiln::gfx::vulkan::check_result(
              window->create_vulkan_surface(vulkan_instance.get())
          ),
          device,
          m_number_of_frames,
          true,
          window->resolution(),
      },
      m_graphics_pipeline_layout{
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
          std::array{ m_surface.surface_format().format },
      }
{
}

auto Renderer::Builder::build(
    const kiln::gfx::vulkan::Instance& vulkan_instance,
    const kiln::gfx::renderer::Device& device,
    Window&                            window,
    const kiln::gfx::renderer::PresentationContext&,
    const kiln::gfx::renderer::PipelineContext&
) -> Renderer
{
    return Renderer{ vulkan_instance, device, window };
}

}   // namespace demo
