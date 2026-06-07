module;

#include <cstdint>
#include <vector>

export module examples.frustum_culling.workflow.Renderer;

import vulkan_hpp;

import kiln.gfx.renderer.command.ComputeCommandBuffer;
import kiln.gfx.renderer.command.ComputeCommandPool;
import kiln.gfx.renderer.command.ComputeQueueRef;
import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.command.GraphicsCommandPool;
import kiln.gfx.renderer.command.GraphicsQueueRef;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Image;
import kiln.gfx.renderer.pipeline.ComputePipeline;
import kiln.gfx.renderer.pipeline.GraphicsPipeline;
import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.renderer.presentation.RenderSurface;

import examples.frustum_culling.Camera;
import examples.frustum_culling.workflow.Scene;

namespace demo {

export class Renderer {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    Renderer(Renderer&&, const allocator_type&);

    explicit Renderer(
        const kiln::gfx::renderer::Device&  device,
        kiln::gfx::renderer::QueueProvider& queue_provider,
        kiln::gfx::renderer::Allocator&     render_allocator,
        uint32_t                            number_of_frames_in_flight,
        vk::Format                          swapchain_surface_format,
        const vk::Extent2D&                 swapchain_surface_extent,
        uint32_t                            number_of_swapchain_images,
        bool                                disable_culling
    );
    explicit Renderer(
        std::allocator_arg_t,
        const allocator_type&               allocator,
        const kiln::gfx::renderer::Device&  device,
        kiln::gfx::renderer::QueueProvider& queue_provider,
        kiln::gfx::renderer::Allocator&     render_allocator,
        uint32_t                            number_of_frames_in_flight,
        vk::Format                          swapchain_surface_format,
        const vk::Extent2D&                 swapchain_surface_extent,
        uint32_t                            number_of_swapchain_images,
        bool                                disable_culling
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    auto resize(
        const kiln::gfx::renderer::Device& device,
        kiln::gfx::renderer::Allocator&    allocator,
        const vk::Extent2D&                swapchain_surface_extent
    ) -> void;

    auto render(
        const kiln::gfx::renderer::Device&  device,
        kiln::gfx::renderer::QueueProvider& queue_provider,
        kiln::gfx::renderer::RenderSurface& surface,
        const Scene&                        scene,
        const Camera&                       camera,
        std::pmr::memory_resource&          transient_memory_resource
    ) -> void;

private:
    bool                                                      m_disable_culling{};
    std::pmr::vector<kiln::gfx::renderer::ComputeCommandPool> m_compute_command_pools;
    std::pmr::vector<kiln::gfx::renderer::ComputeCommandBuffer>
        m_frustum_culling_command_buffers;
    std::pmr::vector<kiln::gfx::renderer::ComputeCommandBuffer>
        m_draw_command_generation_command_buffers;
    std::pmr::vector<kiln::gfx::renderer::ComputeCommandBuffer>
        m_instance_index_generation_command_buffers;
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandPool> m_graphics_command_pools;
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandBuffer>
                                          m_graphics_command_buffers;
    std::pmr::vector<vk::raii::Semaphore> m_image_acquired_semaphores;
    std::pmr::vector<vk::raii::Semaphore> m_render_finished_semaphores;
    std::pmr::vector<vk::raii::Fence>     m_render_finished_fences;
    uint32_t                              m_current_frame_index{};
    kiln::gfx::renderer::Image            m_depth_image;
    vk::raii::ImageView                   m_depth_image_view;
    vk::raii::PipelineLayout              m_frustum_culling_pipeline_layout;
    vk::raii::PipelineLayout              m_draw_command_generation_pipeline_layout;
    vk::raii::PipelineLayout              m_instance_index_generation_pipeline_layout;
    vk::raii::PipelineLayout              m_graphics_pipeline_layout;
    kiln::gfx::renderer::ShaderModule     m_frustum_culling_shader_module;
    kiln::gfx::renderer::ShaderModule     m_draw_command_generation_shader_module;
    kiln::gfx::renderer::ShaderModule     m_instance_index_generation_shader_module;
    kiln::gfx::renderer::ShaderModule     m_graphics_shader_module;
    kiln::gfx::renderer::ComputePipeline  m_frustum_culling_pipeline;
    kiln::gfx::renderer::ComputePipeline  m_draw_command_generation_pipeline;
    kiln::gfx::renderer::ComputePipeline  m_instance_index_generation_pipeline;
    kiln::gfx::renderer::GraphicsPipeline m_graphics_pipeline;


    [[nodiscard]]
    auto number_of_frames_in_flight() const noexcept -> uint32_t;


    auto frustum_cull(
        kiln::gfx::renderer::QueueProvider& queue_provider,
        const Scene&                        scene,
        const Camera&                       camera,
        double                              aspect_ratio,
        std::pmr::memory_resource&          transient_memory_resource
    ) -> void;
    auto generate_draw_commands(
        kiln::gfx::renderer::QueueProvider& queue_provider,
        const Scene&                        scene,
        std::pmr::memory_resource&          transient_memory_resource
    ) -> void;
    auto generate_instance_indices(
        kiln::gfx::renderer::QueueProvider& queue_provider,
        const Scene&                        scene,
        std::pmr::memory_resource&          transient_memory_resource
    ) -> void;

    auto draw(
        kiln::gfx::renderer::QueueProvider&       queue_provider,
        const kiln::gfx::renderer::RenderSurface& surface,
        const Scene&                              scene,
        const Camera&                             camera,
        uint32_t                                  swapchain_image_index,
        std::pmr::memory_resource&                transient_memory_resource
    ) -> void;
};

}   // namespace demo
