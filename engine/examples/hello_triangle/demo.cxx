module;

#include <array>
#include <filesystem>
#include <ranges>
#include <source_location>
#include <span>

#include <vulkan/vulkan.hpp>

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
    const kiln::gfx::renderer::Device&          render_device,
    const kiln::gfx::renderer::GraphicsQueueRef graphics_queue
) -> kiln::gfx::renderer::GraphicsCommandPool
{
    return kiln::gfx::renderer::GraphicsCommandPool{
        render_device,
        graphics_queue.family_index(),
        kiln::gfx::renderer::CommandPoolFlags::eResettable,
    };
}

[[nodiscard]]
auto create_graphics_command_pools(
    const kiln::gfx::renderer::Device&          render_device,
    const kiln::gfx::renderer::GraphicsQueueRef graphics_queue,
    const uint8_t                               number_of_frames
) -> std::vector<kiln::gfx::renderer::GraphicsCommandPool>
{
    std::vector<kiln::gfx::renderer::GraphicsCommandPool> result;
    result.reserve(number_of_frames);

    for (auto _ : std::views::repeat(std::ignore, number_of_frames))
    {
        result.push_back(create_graphics_command_pool(render_device, graphics_queue));
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

[[nodiscard]]
auto create_per_frame_semaphores(
    const kiln::gfx::renderer::Device& device,
    const uint32_t                     number_of_frames
) -> std::vector<vk::raii::Semaphore>
{
    std::vector<vk::raii::Semaphore> result;
    result.reserve(number_of_frames);

    for (const auto _ : std::views::repeat(std::ignore, number_of_frames))
    {
        result.push_back(
            kiln::gfx::vulkan::check_result(
                device.logical_device().createSemaphore(vk::SemaphoreCreateInfo{})
            )
        );
    }

    return result;
}

[[nodiscard]]
auto create_per_frame_fences(
    const kiln::gfx::renderer::Device& device,
    const uint8_t                      number_of_frames
) -> std::vector<vk::raii::Fence>
{
    std::vector<vk::raii::Fence> result;
    result.reserve(number_of_frames);

    for (const auto _ : std::views::repeat(std::ignore, number_of_frames))
    {
        result.push_back(
            kiln::gfx::vulkan::check_result(device.logical_device().createFence(
                vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled }
            ))
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
    : m_render_device_ref{ render_device },
      m_graphics_queue{ *render_queue_provider.graphics_queue() },
      m_window{ create_window(config, wsi_context) },
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
              m_graphics_queue,
              m_number_of_frames
          )   //
      },
      m_graphics_command_buffers{
          create_graphics_command_buffers(m_graphics_command_pools)
      },
      m_image_acquired_semaphores{
          create_per_frame_semaphores(render_device, m_number_of_frames)
      },
      m_render_finished_semaphores{
          create_per_frame_semaphores(render_device, m_surface.number_of_images())
      },
      m_render_finished_fences{ create_per_frame_fences(render_device, m_number_of_frames) }
{
}

auto Demo::window() noexcept -> kiln::wsi::Window&
{
    return m_window;
}

auto Demo::on_window_resize(const kiln::wsi::Size2u new_resolution) -> void
{
    if (m_surface.extent() == new_resolution)
    {
        return;
    }

    m_render_device_ref.get().logical_device().waitIdle();
    m_surface.resize(new_resolution);
}

auto Demo::render() -> void
{
    kiln::gfx::vulkan::check_result(
        m_render_device_ref.get().logical_device().waitForFences(
            *m_render_finished_fences[m_current_frame_index],
            vk::True,
            std::numeric_limits<uint64_t>::max()
        )
    );
    m_render_device_ref.get().logical_device().resetFences(
        *m_render_finished_fences[m_current_frame_index]
    );

    const std::optional<uint32_t> swapchain_image_index =
        m_surface.acquire_image(m_image_acquired_semaphores[m_current_frame_index]);
    if (!swapchain_image_index.has_value())
    {
        return;
    }

    m_graphics_command_pools[m_current_frame_index].reset();
    kiln::gfx::renderer::GraphicsCommandBuffer& graphics_command_buffer{
        m_graphics_command_buffers[m_current_frame_index]
    };

    graphics_command_buffer.begin();

    const vk::ImageMemoryBarrier2 render_image_memory_barrier {
        .srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe,
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .image = m_surface.image_at(*swapchain_image_index),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = 1,
            .layerCount = 1,
          },
    };
    const kiln::gfx::renderer::DependencyInfo render_dependency_info{
        .image_memory_barriers = std::span{ &render_image_memory_barrier, 1 },
    };
    graphics_command_buffer.barrier(render_dependency_info);

    const vk::Rect2D render_area{
        .extent = *m_surface.extent(),
    };
    const kiln::gfx::renderer::RenderPass render_pass{
        render_area,
        std::array{
                   kiln::gfx::renderer::ColorAttachment{
                m_surface.image_view_at(*swapchain_image_index),
            }
                .set_clear_value(std::array{ 0.01f, 0.01f, 0.01f, 1.f }),   //
        }
    };
    graphics_command_buffer.begin_render_pass(render_pass);

    graphics_command_buffer.bind_pipeline(m_pipeline);
    graphics_command_buffer.draw(3);
    graphics_command_buffer.end_render_pass();

    const vk::ImageMemoryBarrier2 present_image_memory_barrier {
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe,
        .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .newLayout = vk::ImageLayout::ePresentSrcKHR,
        .image = m_surface.image_at(*swapchain_image_index),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
          },
    };
    const kiln::gfx::renderer::DependencyInfo present_dependency_info{
        .image_memory_barriers = std::span{ &present_image_memory_barrier, 1 },
    };
    graphics_command_buffer.barrier(present_dependency_info);

    graphics_command_buffer.end();

    const vk::SemaphoreSubmitInfo render_wait_semaphore_info{
        .semaphore = m_image_acquired_semaphores[m_current_frame_index],
    };
    const vk::SemaphoreSubmitInfo render_finished_semaphore_info{
        .semaphore = m_render_finished_semaphores[*swapchain_image_index],
        .stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    };
    m_graphics_queue.submit(
        graphics_command_buffer,
        kiln::gfx::renderer::SubmitInfo{
            .wait_semaphores   = std::span{     &render_wait_semaphore_info, 1 },
            .signal_semaphores = std::span{ &render_finished_semaphore_info, 1 },
            .fence             = m_render_finished_fences[m_current_frame_index],
    }
    );

    m_surface.present(
        m_graphics_queue,
        *swapchain_image_index,
        std::span{ &*m_render_finished_semaphores[*swapchain_image_index], 1 }
    );

    m_current_frame_index = (m_current_frame_index + 1) % m_number_of_frames;
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Demo::shut_down() -> void
{
    m_render_device_ref.get().logical_device().waitIdle();
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
    kiln::gfx::renderer::QueueProviderPlugin& queue_provider_plugin,
    const kiln::gfx::renderer::PipelinePlugin&
) -> DemoPlugin
{
    queue_provider_plugin.require_graphics_queue();

    return DemoPlugin{};
}
