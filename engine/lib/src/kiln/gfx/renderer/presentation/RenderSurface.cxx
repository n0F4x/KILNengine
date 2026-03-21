module;

#include <span>
#include <utility>

module kiln.gfx.renderer.presentation.RenderSurface;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.presentation.Swapchain;

namespace kiln::gfx::renderer {

[[nodiscard]]
constexpr auto pick_surface_format(
    const std::span<const vk::SurfaceFormatKHR> surface_formats
) -> vk::SurfaceFormatKHR
{
    for (const vk::SurfaceFormatKHR& surface_format : surface_formats)
    {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb
            && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return surface_format;
        }
    }
    return surface_formats.front();
}

RenderSurface::RenderSurface(
    vk::raii::SurfaceKHR&& surface,
    const Device&          device,
    const uint8_t          number_of_frames_in_flight,
    const bool             vsync,
    const vk::Extent2D     size
)
    : m_surface{ std::move(surface) },
      m_device_ref{ device },
      m_surface_format{
          pick_surface_format(device.physical_device().getSurfaceFormatsKHR(surface))
      },
      m_number_of_frames_in_flight{ number_of_frames_in_flight },
      m_vsync{ vsync }
{
    resize(size);
}

auto RenderSurface::surface_format() const noexcept -> const vk::SurfaceFormatKHR&
{
    return m_surface_format;
}

auto RenderSurface::resize(const vk::Extent2D size) -> void
{
    if (m_swapchain.has_value())
    {
        m_device_ref.get().logical_device().waitIdle();

        // TODO: destroy swapchain in case any image is currently acquired by the
        //  application
    }

    if (size.width != 0 && size.height != 0)
    {
        m_swapchain = Swapchain{
            m_surface,
            m_surface_format,
            m_device_ref,
            size,
            m_number_of_frames_in_flight,
            m_vsync,
            m_swapchain
                .transform(
                    [](const Swapchain& old_swapchain) -> const Swapchain*
                    { return &old_swapchain; }
                )
                .value_or(nullptr),
        };
    }
}

}   // namespace kiln::gfx::renderer
