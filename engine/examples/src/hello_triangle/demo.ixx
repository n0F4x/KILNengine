module;

#include <functional>
#include <vector>

export module hello_triangle;

import vulkan_hpp;

import kiln;

export class Demo {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;

    class Builder;


    Demo(Demo&&, const allocator_type& allocator);
    Demo(
        std::allocator_arg_t,
        const allocator_type&                     allocator,
        const kiln::app::Config&                  config,
        const kiln::gfx::vulkan::Instance&        vulkan_instance,
        const kiln::wsi::Context&                 wsi_context,
        const kiln::gfx::renderer::Device&        render_device,
        const kiln::gfx::renderer::QueueProvider& render_queue_provider
    );

    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;

    [[nodiscard]]
    auto window() noexcept -> kiln::wsi::Window&;

    auto on_window_resize(kiln::wsi::Size2u new_resolution) -> void;

    auto render() -> void;

    auto shut_down() -> void;

private:
    std::reference_wrapper<const kiln::gfx::renderer::Device>  m_render_device_ref;
    kiln::gfx::renderer::GraphicsQueueRef                      m_graphics_queue;
    uint8_t                                                    m_number_of_frames{ 2 };
    uint8_t                                                    m_current_frame_index{};
    kiln::wsi::Window                                          m_window;
    kiln::gfx::renderer::RenderSurface                         m_surface;
    vk::raii::PipelineLayout                                   m_pipeline_layout;
    kiln::gfx::renderer::ShaderModule                          m_shader_module;
    kiln::gfx::renderer::GraphicsPipeline                      m_pipeline;
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandPool> m_graphics_command_pools;
    std::pmr::vector<kiln::gfx::renderer::GraphicsCommandBuffer> m_graphics_command_buffers;
    std::pmr::vector<vk::raii::Semaphore> m_image_acquired_semaphores;
    std::pmr::vector<vk::raii::Semaphore> m_render_finished_semaphores;
    std::pmr::vector<vk::raii::Fence>     m_render_finished_fences;
};

class Demo::Builder : public kiln::app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto create(
        kiln::gfx::renderer::DeviceBuilder&        device_builder,
        const kiln::gfx::renderer::CommandContextBuilder& command_context_builder,
        const kiln::gfx::renderer::PresentationContextBuilder& presentation_context_builder,
        const kiln::gfx::renderer::PipelineContextBuilder&
    ) -> Builder;

    [[nodiscard]]
    static auto build(
        kiln::app::Arena&                         arena,
        const kiln::app::Config&                  config,
        const kiln::gfx::vulkan::Instance&        vulkan_instance,
        const kiln::wsi::Context&                 wsi_context,
        const kiln::gfx::renderer::Device&        render_device,
        const kiln::gfx::renderer::QueueProvider& render_queue_provider
    ) -> Demo;
};
