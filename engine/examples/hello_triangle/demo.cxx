module;

#include <array>
#include <filesystem>
#include <ranges>
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

[[nodiscard]]
auto create_graphics_command_pool(
    const kiln::gfx::renderer::Device&        render_device,
    const kiln::gfx::renderer::QueueProvider& render_queue_provider
) -> kiln::gfx::renderer::GraphicsCommandPool
{
    return kiln::gfx::renderer::GraphicsCommandPool{
        render_device,
        render_queue_provider.graphics_queue()->family_index(),
        kiln::gfx::renderer::CommandPoolFlags::eResettable,
    };
}

[[nodiscard]]
auto create_graphics_command_pools(
    const kiln::gfx::renderer::Device&        render_device,
    const kiln::gfx::renderer::QueueProvider& render_queue_provider,
    const uint8_t                             number_of_frames
) -> std::vector<kiln::gfx::renderer::GraphicsCommandPool>
{
    std::vector<kiln::gfx::renderer::GraphicsCommandPool> result;
    result.reserve(number_of_frames);

    for (auto _ : std::views::repeat(std::ignore, number_of_frames))
    {
        result.push_back(
            create_graphics_command_pool(render_device, render_queue_provider)
        );
    }

    return result;
}

[[nodiscard]]
auto create_graphics_command_buffers(
    std::vector<kiln::gfx::renderer::GraphicsCommandPool>& command_pools
) -> std::vector<kiln::gfx::renderer::GraphicsCommandBuffer>
{
    std::vector<kiln::gfx::renderer::GraphicsCommandBuffer> result;
    result.reserve(command_pools.size());

    for (const std::size_t index : std::views::iota(0uz, command_pools.size()))
    {
        result.push_back(
            command_pools[index].allocate_primary(
                kiln::gfx::renderer::CommandBufferUsageFlags::eReusable
            )
        );
    }

    return result;
}

Demo::Demo(
    const kiln::config::Config&               config,
    const vk::raii::Instance&                 vulkan_instance,
    const kiln::wsi::Context&                 wsi_context,
    const kiln::gfx::renderer::Device&        render_device,
    const kiln::gfx::renderer::QueueProvider& render_queue_provider
)
    : m_window{ create_window(config, wsi_context) },
      m_surface{
          kiln::gfx::vulkan::check_result(m_window.create_vulkan_surface(vulkan_instance)),
          render_device,
          m_number_of_frames,
          true,
          m_window.resolution(),
      },
      m_pipeline_layout{
          kiln::gfx::vulkan::check_result(
              render_device.logical_device().createPipelineLayout({})
          )   //
      },
      m_shader_module{
          *kiln::gfx::renderer::ShaderModule::load_from_file(
              std::filesystem::path{ std::source_location::current().file_name() }
                  .parent_path()
              / "shaders" / "triangle.spv"
          )   //
      },
      m_pipeline{
          render_device,
          m_pipeline_layout,
          m_shader_module,
          m_shader_module,
          std::array{ m_surface.surface_format().format },
      },
      m_graphics_command_pools{
          create_graphics_command_pools(
              render_device,
              render_queue_provider,
              m_number_of_frames
          )   //
      },
      m_graphics_command_buffers{
          create_graphics_command_buffers(m_graphics_command_pools)
      }
{
}

auto Demo::window() noexcept -> kiln::wsi::Window&
{
    return m_window;
}

auto Demo::render() -> void
{
    m_graphics_command_pools[m_current_frame_index].reset();
    kiln::gfx::renderer::GraphicsCommandBuffer& graphics_command_buffer{
        m_graphics_command_buffers[m_current_frame_index]
    };

    graphics_command_buffer.begin();

    const vk::Rect2D render_area{
        .extent = m_window.resolution(),
    };
    const kiln::gfx::renderer::RenderPass render_pass{
        render_area,
        std::array{
                   kiln::gfx::renderer::ColorAttachment{}.set_clear_value(
                std::array{ 0.f, 0.f, 0.2f, 1.f }
            ),   //
        }
    };
    graphics_command_buffer.begin_render_pass(render_pass);

    graphics_command_buffer.bind_pipeline(m_pipeline);
    graphics_command_buffer.draw(3);
    graphics_command_buffer.end_render_pass();
    graphics_command_buffer.end();


    m_current_frame_index = (m_current_frame_index + 1) % m_number_of_frames;
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
