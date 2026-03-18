module;

#include <array>
#include <filesystem>
#include <source_location>
#include <span>

module demo;

import kiln;

[[nodiscard]]
auto create_window(
    const kiln::config::Config& config,
    const kiln::wsi::Context&   wsi_context
) -> kiln::wsi::Window
{
    constexpr static kiln::wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    const kiln::wsi::Window::CreateInfo window_info{
        .title    = config.app_name(),
        .settings = screen_settings,
    };

    return kiln::wsi::Window{ wsi_context, window_info };
}

Demo::Demo(
    const kiln::config::Config&               config,
    const vk::raii::Instance&                 vulkan_instance,
    const kiln::wsi::Context&                 wsi_context,
    const kiln::gfx::renderer::Device&        render_device,
    const kiln::gfx::renderer::QueueProvider& render_queue_provider
)
    : window{ create_window(config, wsi_context) },
      surface{
          kiln::gfx::vulkan::check_result(window.create_vulkan_surface(vulkan_instance))
      },
      swapchain{ render_device, surface, window.resolution(), 2, true },
      pipeline_layout{
          kiln::gfx::vulkan::check_result(
              render_device.logical_device().createPipelineLayout({})
          )   //
      },
      shader_module{
          *kiln::gfx::renderer::ShaderModule::load_from_file(
              std::filesystem::path{ std::source_location::current().file_name() }
                  .parent_path()
              / "shaders" / "triangle.spv"
          )   //
      },
      pipeline{
          render_device,
          pipeline_layout,
          shader_module,
          shader_module,
          std::array{ swapchain.surface_format().format },
      },
      graphics_command_pool{
          render_device,
          render_queue_provider.graphics_queue()->family_index(),
          kiln::gfx::renderer::CommandPoolFlags::eResettable,
      },
      graphics_command_buffer{
          graphics_command_pool.allocate_primary(
              kiln::gfx::renderer::CommandBufferUsageFlags::eResettable
          )   //
      }
{
}

auto Demo::render() -> void
{
    graphics_command_buffer.begin();

    const vk::Rect2D render_area{
        .extent = window.resolution(),
    };
    const vk::RenderingAttachmentInfo color_attachment{
        // .imageView   = *swapchain.current_image_view(),
        .imageLayout = vk::ImageLayout::eAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eStore,
        .clearValue{ .color{ std::array{ 0.f, 0.f, 0.2f, 1.f } } },
    };
    const kiln::gfx::renderer::RenderPass render_pass{
        render_area, std::span{ &color_attachment, 1 }
    };
    graphics_command_buffer.begin_rendering(render_pass);

    graphics_command_buffer.bind_pipeline(pipeline);
    graphics_command_buffer.draw(3);
    graphics_command_buffer.end_rendering();
    graphics_command_buffer.end();
}

auto DemoPlugin::operator()(
    const kiln::config::Config&               config,
    const vk::raii::Instance&                 vulkan_instance,
    const kiln::wsi::Context&                 wsi_context,
    const kiln::gfx::renderer::Device&        render_device,
    const kiln::gfx::renderer::QueueProvider& render_queue_provider
) -> Demo
{
    return Demo{
        config, vulkan_instance, wsi_context, render_device, render_queue_provider
    };
}

auto demo_plugin_injection(
    kiln::gfx::vulkan::InstancePlugin&        instance_plugin,
    kiln::gfx::renderer::DevicePlugin&        device_plugin,
    kiln::gfx::renderer::QueueProviderPlugin& queue_provider_plugin,
    const kiln::gfx::renderer::PipelinePlugin&
) -> DemoPlugin
{
    instance_plugin->target_api_version(vk::ApiVersion13);
    device_plugin->require_minimum_version(vk::ApiVersion13);
    queue_provider_plugin.require_graphics_queue();
    device_plugin->enable_features(
        vk::PhysicalDeviceSynchronization2Features{ .synchronization2 = vk::True }
    );

    return DemoPlugin{};
}
