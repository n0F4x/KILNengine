module;

#include <functional>
#include <optional>
#include <span>

export module kiln.gfx.renderer.presentation.RenderSurface;

import vulkan_hpp;

import kiln.gfx.renderer.command.QueueRefBase;
import kiln.gfx.renderer.command.SubmitInfo;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.presentation.Swapchain;
import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

export class RenderSurface {
public:
    RenderSurface(
        vk::raii::SurfaceKHR&& surface,
        const Device&          device,
        uint8_t                number_of_frames_in_flight,
        bool                   vsync,
        vk::Extent2D           size
    );

    [[nodiscard]]
    auto surface_format() const noexcept -> const vk::SurfaceFormatKHR&;
    [[nodiscard]]
    auto extent() const noexcept -> std::optional<vk::Extent2D>;
    [[nodiscard]]
    auto number_of_images() const noexcept -> uint32_t;
    [[nodiscard]]
    auto image_view_at(uint32_t index) const noexcept -> const vk::raii::ImageView&;

    auto resize(vk::Extent2D size) -> void;

    [[nodiscard]]
    auto acquire_image(
        util::OptionalRef<const vk::raii::Semaphore> signal_semaphore,
        util::OptionalRef<const vk::raii::Fence>     fence = std::nullopt
    ) -> std::optional<uint32_t>;

    auto present(
        QueueRefBase                   queue,
        uint32_t                       image_index,
        std::span<const vk::Semaphore> wait_semaphores
    ) -> bool;

private:
    vk::raii::SurfaceKHR                 m_surface;
    std::reference_wrapper<const Device> m_device_ref;
    vk::SurfaceFormatKHR                 m_surface_format;
    uint8_t                              m_number_of_frames_in_flight;
    bool                                 m_vsync;
    std::optional<Swapchain>             m_swapchain;
};

}   // namespace kiln::gfx::renderer
