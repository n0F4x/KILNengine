module;

#include <array>
#include <cstdint>
#include <filesystem>
#include <source_location>
#include <span>

#include <vk_mem_alloc.h>

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

[[nodiscard]]
auto create_index_buffer(
    const kiln::gfx::renderer::Device&        render_device,
    const kiln::gfx::renderer::Allocator&     render_allocator,
    kiln::gfx::renderer::TransferCommandPool& transfer_command_pool
) -> kiln::gfx::renderer::Buffer
{
    constexpr static std::array<uint16_t, 3> indices{ 0, 1, 2 };
    constexpr static uint32_t buffer_size{ (std::span{ indices }.size_bytes()) };

    constexpr static vk::BufferCreateInfo staging_buffer_create_info{
        .size  = buffer_size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    auto [staging_buffer, staging_allocation, _]{
        render_allocator.create_buffer(
            staging_buffer_create_info,
            VmaAllocationCreateInfo{
                .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
                       | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                .usage = VMA_MEMORY_USAGE_AUTO,
            }
        )   //
    };

    std::memcpy(staging_allocation.map().data(), indices.data(), buffer_size);
    if (!(staging_allocation.memory_properties()
          & vk::MemoryPropertyFlagBits::eHostCoherent))
    {
        staging_allocation.flush(0, buffer_size);
    }
    staging_allocation.unmap();

    constexpr static vk::BufferCreateInfo buffer_create_info = {
        .size  = buffer_size,
        .usage = vk::BufferUsageFlagBits::eIndexBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    auto [buffer, allocation, _]{
        render_allocator.create_buffer(
            buffer_create_info,
            VmaAllocationCreateInfo{
                .usage = VMA_MEMORY_USAGE_AUTO,
            }
        )   //
    };

    kiln::gfx::renderer::TransferCommandBuffer command_buffer{
        transfer_command_pool.allocate_primary()
    };
    command_buffer.begin();
    command_buffer.enqueue_buffer_copy(staging_buffer, buffer);
    command_buffer.end();

    const vk::raii::Fence fence{
        kiln::gfx::vulkan::check_result(
            render_device.logical_device().createFence(vk::FenceCreateInfo{})
        )   //
    };
    render_device.host_to_device_transfer_queue().submit(
        command_buffer, kiln::gfx::renderer::SubmitInfo{ .fence = fence }
    );
    kiln::gfx::vulkan::check_result(render_device.logical_device().waitForFences(
        std::array{ *fence }, vk::True, 100'000'000'000
    ));

    return std::move(buffer);
}

Demo::Demo(
    const kiln::config::Config&           config,
    const vk::raii::Instance&             vulkan_instance,
    const kiln::wsi::Context&             wsi_context,
    const kiln::gfx::renderer::Device&    render_device,
    const kiln::gfx::renderer::Allocator& render_allocator
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
          std::array{ window.surface_format().format },
      },
      index_buffer{
          create_index_buffer(
              render_device,
              render_allocator,
              immediate_transfer_command_pool
          )   //
      }
{
}

auto DemoPlugin::operator()(kiln::app::App& app) -> void
{
    app.context().insert(
        Demo{
            app.context().at<kiln::config::Config>(),
            app.context().at<vk::raii::Instance>(),
            app.context().at<kiln::wsi::Context>(),
            app.context().at<kiln::gfx::renderer::Device>(),
            app.context().at<kiln::gfx::renderer::Allocator>(),
        }
    );
}

auto demo_plugin_injection(
    const kiln::config::Plugin&,
    kiln::gfx::vulkan::InstancePlugin& instance_plugin,
    const kiln::wsi::Plugin&,
    kiln::gfx::renderer::DevicePlugin& device_plugin,
    const kiln::gfx::renderer::AllocatorPlugin&,
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
