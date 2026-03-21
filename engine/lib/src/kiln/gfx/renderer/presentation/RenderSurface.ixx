module;

#include <functional>
#include <optional>

export module kiln.gfx.renderer.presentation.RenderSurface;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.presentation.Swapchain;

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

    auto resize(vk::Extent2D size) -> void;

private:
    vk::raii::SurfaceKHR                 m_surface;
    std::reference_wrapper<const Device> m_device_ref;
    vk::SurfaceFormatKHR                 m_surface_format;
    uint8_t                              m_number_of_frames_in_flight;
    bool                                 m_vsync;
    std::optional<Swapchain>             m_swapchain;
};

}   // namespace kiln::gfx::renderer
