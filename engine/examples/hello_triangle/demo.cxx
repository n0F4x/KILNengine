module;

#include <array>
#include <filesystem>
#include <source_location>
#include <span>

module demo;

import kiln;

[[nodiscard]]
auto create_window(
    const kiln::config::Config&        config,
    const vk::raii::Instance&          vulkan_instance,
    const kiln::wsi::Context&          wsi_context,
    const kiln::gfx::renderer::Device& render_device
) -> kiln::wsi::VulkanWindow
{
    constexpr static kiln::wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    const kiln::wsi::Window::CreateInfo window_info{
        .title    = config.app_name(),
        .settings = screen_settings,
    };

    return render_device.create_window(vulkan_instance, wsi_context, window_info);
}

Demo::Demo(
    const kiln::config::Config&        config,
    const vk::raii::Instance&          vulkan_instance,
    const kiln::wsi::Context&          wsi_context,
    const kiln::gfx::renderer::Device& render_device
)
    : immediate_transfer_command_pool{
          render_device,
          render_device.host_to_device_transfer_queue().family_index()
      },
      window{ create_window(config, vulkan_instance, wsi_context, render_device) },
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
          std::array{ window.swapchain().surface_format().format },
      },
      graphics_command_pool{
          render_device,
          render_device.graphics_queue().family_index(),
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
        .extent{
                .width  = window.resolution().width,
                .height = window.resolution().height,
                },
    };
    const vk::RenderingAttachmentInfo color_attachment{
        // .imageView   = window.swapchain().swapchain_image_views()[image_index],
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

auto DemoPlugin::operator()(kiln::app::App& app) -> void
{
    app.context().insert(
        Demo{
            app.context().at<kiln::config::Config>(),
            app.context().at<vk::raii::Instance>(),
            app.context().at<kiln::wsi::Context>(),
            app.context().at<kiln::gfx::renderer::Device>(),
        }
    );
}

auto demo_plugin_injection(
    const kiln::config::Plugin&,
    kiln::gfx::vulkan::InstancePlugin& instance_plugin,
    const kiln::wsi::Plugin&,
    kiln::gfx::renderer::DevicePlugin& device_plugin,
    const kiln::gfx::renderer::PipelinePlugin&
) -> DemoPlugin
{
    instance_plugin->target_api_version(vk::ApiVersion13);
    device_plugin->require_minimum_version(vk::ApiVersion13);
    device_plugin->request_graphics_queue();
    device_plugin->request_host_to_device_transfer_queue();
    device_plugin->enable_features(
        vk::PhysicalDeviceSynchronization2Features{ .synchronization2 = vk::True }
    );

    return DemoPlugin{};
}
