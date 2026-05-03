module;

#include <cstdint>
#include <vector>

export module examples.simple_scene.workflow.Renderer;

import vulkan_hpp;

import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.command.GraphicsCommandPool;
import kiln.gfx.renderer.command.GraphicsQueue;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.pipeline.GraphicsPipeline;
import kiln.gfx.renderer.pipeline.ShaderModule;
import kiln.gfx.renderer.presentation.RenderSurface;

import examples.simple_scene.Camera;
import examples.simple_scene.workflow.Scene;

namespace demo {

export class Renderer {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    Renderer(Renderer&&, const allocator_type&);

    explicit Renderer(
        const kiln::gfx::renderer::Device& device,
        uint32_t                           number_of_frames_in_flight,
        vk::Format                         swapchain_surface_format,
        uint32_t                           number_of_swapchain_images
    );
    explicit Renderer(
        std::allocator_arg_t,
        const allocator_type&              allocator,
        const kiln::gfx::renderer::Device& device,
        uint32_t                           number_of_frames_in_flight,
        vk::Format                         swapchain_surface_format,
        uint32_t                           number_of_swapchain_images
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    auto render(
        const kiln::gfx::renderer::Device&  device,
        kiln::gfx::renderer::GraphicsQueue& graphics_queue,
        kiln::gfx::renderer::RenderSurface& surface,
        const Scene&                        scene,
        const Camera&                       camera,
        std::pmr::memory_resource&          transient_memory_resource
    ) -> void;

private:
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandPool> m_graphics_command_pools;
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandBuffer>
                                          m_scene_pass_command_buffers;
    std::pmr::vector<vk::raii::Semaphore> m_image_acquired_semaphores;
    std::pmr::vector<vk::raii::Semaphore> m_render_finished_semaphores;
    std::pmr::vector<vk::raii::Fence>     m_render_finished_fences;
    uint32_t                              m_current_frame_index{};
    vk::raii::PipelineLayout              m_pipeline_layout;
    kiln::gfx::renderer::ShaderModule     m_graphics_shader_module;
    kiln::gfx::renderer::GraphicsPipeline m_graphics_pipeline;


    [[nodiscard]]
    auto number_of_frames_in_flight() const noexcept -> uint32_t;

    auto draw_scene(
        kiln::gfx::renderer::GraphicsQueue&       graphics_queue,
        const kiln::gfx::renderer::RenderSurface& surface,
        const Scene&                              scene,
        const Camera&                             camera,
        uint32_t                                  swapchain_image_index,
        std::pmr::memory_resource&                transient_memory_resource
    ) -> void;
};

}   // namespace demo
