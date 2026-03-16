module;

#include <cstdint>
#include <vector>

export module kiln.gfx.renderer.swapchain.Swapchain;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

export class Swapchain {
public:
    Swapchain(
        const Device&               device,
        const vk::raii::SurfaceKHR& surface,
        vk::Extent2D                framebuffer_size,
        std::uint32_t               number_of_frames,
        bool                        enable_vsync
    );

    [[nodiscard]]
    auto surface_format() const noexcept -> const vk::SurfaceFormatKHR&;

private:
    vk::SurfaceFormatKHR             m_surface_format;
    vk::raii::SwapchainKHR           m_swapchain;
    std::vector<vk::Image>           m_swapchain_images;
    std::vector<vk::raii::ImageView> m_swapchain_image_views;
};

}   // namespace kiln::gfx::renderer
