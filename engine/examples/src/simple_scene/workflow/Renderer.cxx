module;

#include <array>
#include <filesystem>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <source_location>
#include <utility>

#include <vk_mem_alloc.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "kiln/util/contract_macros.hpp"

module examples.simple_scene.workflow.Renderer;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandPoolFlags;
import kiln.gfx.renderer.command.DependencyInfo;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Image;
import kiln.gfx.renderer.pipeline.ColorAttachment;
import kiln.gfx.renderer.pipeline.DepthAttachment;
import kiln.gfx.renderer.pipeline.RenderPass;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;

import examples.simple_scene.shaders;

namespace demo {

Renderer::Renderer(Renderer&& other, [[maybe_unused]] const allocator_type& allocator)
    : Renderer{ std::move(other) }
{
    PRECOND(get_allocator() == allocator);
}

Renderer::Renderer(
    const kiln::gfx::renderer::Device& device,
    kiln::gfx::renderer::Allocator&    render_allocator,
    const uint32_t                     number_of_frames_in_flight,
    const vk::Format                   swapchain_surface_format,
    const vk::Extent2D&                swapchain_surface_extent,
    const uint32_t                     number_of_swapchain_images
)
    : Renderer{
          std::allocator_arg,   //
          std::pmr::get_default_resource(),
          device,
          render_allocator,
          number_of_frames_in_flight,
          swapchain_surface_format,
          swapchain_surface_extent,
          number_of_swapchain_images,
      }
{
}

[[nodiscard]]
auto create_graphics_command_pool(const kiln::gfx::renderer::Device& render_device)
    -> kiln::gfx::renderer::GraphicsCommandPool
{
    return kiln::gfx::renderer::GraphicsCommandPool{
        render_device,
        render_device.graphics_queue_info()->family_index,
        kiln::gfx::renderer::CommandPoolFlags::eResettable,
    };
}

[[nodiscard]]
auto create_graphics_command_pools(
    const std::pmr::polymorphic_allocator<>& allocator,
    const kiln::gfx::renderer::Device&       render_device,
    const uint32_t                           count
) -> std::pmr::vector<kiln::gfx::renderer::GraphicsCommandPool>
{
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandPool> result{ allocator };
    result.reserve(count);

    for (auto _ : std::views::repeat(std::ignore, count))
    {
        result.push_back(create_graphics_command_pool(render_device));
    }

    return result;
}

[[nodiscard]]
auto create_graphics_command_buffers(
    const std::pmr::polymorphic_allocator<>&                    allocator,
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandPool>& command_pools
) -> std::pmr::vector<kiln::gfx::renderer::GraphicsCommandBuffer>
{
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandBuffer> result{ allocator };
    result.reserve(command_pools.size());

    for (const std::size_t index : std::views::iota(0uz, command_pools.size()))
    {
        result.push_back(command_pools[index].allocate_primary());
    }

    return result;
}

[[nodiscard]]
auto create_per_frame_semaphores(
    const std::pmr::polymorphic_allocator<>& allocator,
    const kiln::gfx::renderer::Device&       device,
    const uint32_t                           count
) -> std::pmr::vector<vk::raii::Semaphore>
{
    std::pmr::vector<vk::raii::Semaphore> result{ allocator };
    result.reserve(count);

    for (const auto _ : std::views::repeat(std::ignore, count))
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
    const std::pmr::polymorphic_allocator<>& allocator,
    const kiln::gfx::renderer::Device&       device,
    const uint32_t                           count
) -> std::pmr::vector<vk::raii::Fence>
{
    std::pmr::vector<vk::raii::Fence> result{ allocator };
    result.reserve(count);

    for (const auto _ : std::views::repeat(std::ignore, count))
    {
        result.push_back(
            kiln::gfx::vulkan::check_result(device.logical_device().createFence(
                vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled }
            ))
        );
    }

    return result;
}

[[nodiscard]]
auto create_graphics_pipeline_layout(const vk::raii::Device& device)
    -> vk::raii::PipelineLayout
{
    constexpr static std::array push_constant_ranges{
        vk::PushConstantRange{ .stageFlags = vk::ShaderStageFlagBits::eVertex,
                              .size = sizeof(shaders::Scene) + sizeof(shaders::Camera) },
    };

    constexpr static vk::PipelineLayoutCreateInfo create_info{
        .pushConstantRangeCount = push_constant_ranges.size(),
        .pPushConstantRanges    = push_constant_ranges.data(),
    };

    return kiln::gfx::vulkan::check_result(device.createPipelineLayout(create_info));
}

[[nodiscard]]
auto pick_depth_format(const vk::raii::PhysicalDevice& physical_device) -> vk::Format
{
    for (const vk::Format format :
         { vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint })
    {
        if (physical_device.getFormatProperties2(format)
                .formatProperties.optimalTilingFeatures
            & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            return format;
        }
    }
    std::unreachable();
}

[[nodiscard]]
auto create_depth_image(
    const kiln::gfx::renderer::Device& device,
    kiln::gfx::renderer::Allocator&    allocator,
    const vk::Extent2D&                extent
) -> kiln::gfx::renderer::Image
{
    const vk::ImageCreateInfo create_info{
        .imageType = vk::ImageType::e2D,
        .format    = pick_depth_format(device.physical_device()),
        .extent{ .width = extent.width, .height = extent.height, .depth = 1 },
        .mipLevels   = 1,
        .arrayLayers = 1,
        .samples     = vk::SampleCountFlagBits::e1,
        .tiling      = vk::ImageTiling::eOptimal,
        .usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment,
    };
    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };
    return allocator.create_image(create_info, allocation_create_info);
}

[[nodiscard]]
auto create_depth_image_view(
    const kiln::gfx::renderer::Device& device,
    kiln::gfx::renderer::Image&        depth_image
) -> vk::raii::ImageView
{
    const vk::ImageViewCreateInfo create_info{
        .image    = depth_image.get(),
        .viewType = vk::ImageViewType::e2D,
        .format   = depth_image.format(),
        .subresourceRange{ .aspectMask = vk::ImageAspectFlagBits::eDepth,
                          .levelCount = 1,
                          .layerCount = 1 },
    };
    return kiln::gfx::vulkan::check_result(
        device.logical_device().createImageView(create_info)
    );
}

Renderer::Renderer(
    std::allocator_arg_t,
    const allocator_type&              allocator,
    const kiln::gfx::renderer::Device& device,
    kiln::gfx::renderer::Allocator&    render_allocator,
    const uint32_t                     number_of_frames_in_flight,
    const vk::Format                   swapchain_surface_format,
    const vk::Extent2D&                swapchain_surface_extent,
    const uint32_t                     number_of_swapchain_images
)
    : m_graphics_command_pools{
          create_graphics_command_pools(allocator, device, number_of_frames_in_flight)
      },
      m_scene_pass_command_buffers{
          create_graphics_command_buffers(allocator, m_graphics_command_pools)
      },
      m_image_acquired_semaphores{
          create_per_frame_semaphores(allocator, device, number_of_frames_in_flight)
      },
      m_render_finished_semaphores{
          create_per_frame_semaphores(
              allocator,
              device,
              number_of_swapchain_images
          )   //
      },
      m_render_finished_fences{
          create_per_frame_fences(allocator, device, number_of_frames_in_flight)
      },
      m_depth_image{
          create_depth_image(device, render_allocator, swapchain_surface_extent),
      },
      m_depth_image_view{ create_depth_image_view(device, m_depth_image) },
      m_pipeline_layout{ create_graphics_pipeline_layout(device.logical_device()) },
      m_graphics_shader_module{
          *kiln::gfx::renderer::ShaderModule::load_from_file(
              std::filesystem::path{ std::source_location::current().file_name() }
                  .parent_path()
                  .parent_path()
              / "shaders"
              / "main.spv"
          )   //
      },
      m_graphics_pipeline{
          device,
          m_pipeline_layout,
          m_graphics_shader_module,
          m_graphics_shader_module,
          std::array{ swapchain_surface_format },
          pick_depth_format(device.physical_device()),
      }
{
}

auto Renderer::get_allocator() const noexcept -> allocator_type
{
    return m_graphics_command_pools.get_allocator();
}

auto Renderer::resize(
    const kiln::gfx::renderer::Device& device,
    kiln::gfx::renderer::Allocator&    allocator,
    const vk::Extent2D&                swapchain_surface_extent
) -> void
{
    m_depth_image      = create_depth_image(device, allocator, swapchain_surface_extent);
    m_depth_image_view = create_depth_image_view(device, m_depth_image);
}

auto Renderer::render(
    const kiln::gfx::renderer::Device&  device,
    kiln::gfx::renderer::GraphicsQueue& graphics_queue,
    kiln::gfx::renderer::RenderSurface& surface,
    const Scene&                        scene,
    const Camera&                       camera,
    std::pmr::memory_resource&          transient_memory_resource
) -> void
{
    kiln::gfx::vulkan::check_result(device.logical_device().waitForFences(
        *m_render_finished_fences[m_current_frame_index],
        vk::True,
        std::numeric_limits<uint64_t>::max()
    ));

    const std::optional<uint32_t> swapchain_image_index
        = surface.acquire_image(m_image_acquired_semaphores[m_current_frame_index]);
    if (!swapchain_image_index.has_value())
    {
        return;
    }

    device.logical_device().resetFences(*m_render_finished_fences[m_current_frame_index]);


    draw_scene(
        graphics_queue,
        surface,
        scene,
        camera,
        *swapchain_image_index,
        transient_memory_resource
    );


    surface.present(
        graphics_queue,
        *swapchain_image_index,
        std::span{ &*m_render_finished_semaphores[*swapchain_image_index], 1 }
    );


    m_current_frame_index = (m_current_frame_index + 1) % number_of_frames_in_flight();
}

auto Renderer::number_of_frames_in_flight() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_graphics_command_pools.size());
}

auto Renderer::draw_scene(
    kiln::gfx::renderer::GraphicsQueue&       graphics_queue,
    const kiln::gfx::renderer::RenderSurface& surface,
    const Scene&                              scene,
    const Camera&                             camera,
    const uint32_t                            swapchain_image_index,
    std::pmr::memory_resource&                transient_memory_resource
) -> void
{
    m_graphics_command_pools[m_current_frame_index].reset();

    kiln::gfx::renderer::GraphicsCommandBuffer& graphics_command_buffer{
        m_scene_pass_command_buffers[m_current_frame_index]
    };

    graphics_command_buffer.begin_recording();

    const std::array render_image_memory_barriers{
        vk::ImageMemoryBarrier2{
            .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
            .oldLayout = vk::ImageLayout::eUndefined,
            .newLayout = vk::ImageLayout::eAttachmentOptimal,
            .image = surface.image_at(swapchain_image_index),
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .levelCount = 1,
                .layerCount = 1,
              },
        },
        vk::ImageMemoryBarrier2{
            .srcStageMask = vk::PipelineStageFlagBits2::eLateFragmentTests,
            .srcAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            .dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests,
            .dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            .oldLayout = vk::ImageLayout::eUndefined,
            .newLayout = vk::ImageLayout::eAttachmentOptimal,
            .image = m_depth_image.get(),
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil,
                .levelCount = 1,
                .layerCount = 1,
              },
        },
    };
    const kiln::gfx::renderer::DependencyInfo render_dependency_info{
        .image_memory_barriers = std::span{ render_image_memory_barriers },
    };
    graphics_command_buffer.record_barrier(render_dependency_info);

    const vk::Rect2D render_area{
        .extent = *surface.extent(),
    };
    const kiln::gfx::renderer::RenderPass render_pass{
        render_area,
        std::array{
                   kiln::gfx::renderer::ColorAttachment{
                   surface.image_view_at(swapchain_image_index),
                   }
                   .set_clear_value(std::array{ 0.01f, 0.01f, 0.01f, 1.f }),   //
        },
        kiln::gfx::renderer::DepthAttachment{ m_depth_image_view }
        .set_clear_value(1)
    };
    graphics_command_buffer.record_render_pass_start(render_pass);


    if (scene.max_draw_count() != 0)
    {
        const shaders::Scene shader_scene{
            .geometry_buffer_address     = scene.geometry_buffer_address(),
            .material_buffer_address     = scene.material_buffer_address(),
            .instance_buffer_address     = scene.instance_buffer_address(),
            .draw_command_buffer_address = scene.draw_command_buffer_address(),
        };
        const vk::PushConstantsInfo scene_push_constants_info{
            .layout     = m_pipeline_layout,
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .size       = sizeof(decltype(shader_scene)),
            .pValues    = &shader_scene,
        };
        const shaders::Camera shader_camera{
            .position = glm::vec4{ camera.position(), 1.0 },
            .view_projection_matrix
            = glm::perspective(
                  camera.fov(),
                  camera.aspect_ratio(),
                  camera.near_plane(),
                  camera.far_plane()
              )
            * glm::mat4_cast(glm::conjugate(camera.orientation()))
            * glm::translate(glm::identity<glm::dmat4>(), -camera.position()),
        };
        const vk::PushConstantsInfo camera_push_constants_info{
            .layout     = m_pipeline_layout,
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .offset     = sizeof(shaders::Scene),
            .size       = sizeof(decltype(shader_camera)),
            .pValues    = &shader_camera,
        };
        graphics_command_buffer.record_push_constants(scene_push_constants_info);
        graphics_command_buffer.record_push_constants(camera_push_constants_info);
        graphics_command_buffer.record_pipeline_bind(m_graphics_pipeline);
        graphics_command_buffer.record_indirect_draw_count(
            scene.draw_command_buffer_region(),
            scene.draw_command_count_buffer_region(),
            scene.max_draw_count(),
            sizeof(shaders::DrawCommand)
        );
    }


    graphics_command_buffer.record_render_pass_finish();

    const vk::ImageMemoryBarrier2 present_image_memory_barrier {
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eAllCommands,
        .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .newLayout = vk::ImageLayout::ePresentSrcKHR,
        .image = surface.image_at(swapchain_image_index),
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
    graphics_command_buffer.record_barrier(present_dependency_info);

    graphics_command_buffer.end_recording();


    const vk::SemaphoreSubmitInfo render_wait_semaphore_info{
        .semaphore = m_image_acquired_semaphores[m_current_frame_index],
        .stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    };
    const vk::SemaphoreSubmitInfo render_finished_semaphore_info{
        .semaphore = m_render_finished_semaphores[swapchain_image_index],
        .stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    };
    kiln::gfx::renderer::SubmitInfo submit_info{ &transient_memory_resource };
    submit_info.wait_semaphores().push_back(render_wait_semaphore_info);
    submit_info.signal_semaphores().push_back(render_finished_semaphore_info);
    submit_info.fence() = *m_render_finished_fences[m_current_frame_index];
    graphics_queue.submit(graphics_command_buffer, submit_info);
}

}   // namespace demo
