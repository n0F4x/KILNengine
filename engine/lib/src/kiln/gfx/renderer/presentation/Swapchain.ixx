module;

#include <cstdint>
#include <optional>
#include <vector>

export module kiln.gfx.renderer.presentation.Swapchain;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

export class Swapchain {
public:
    Swapchain(
        const vk::raii::SurfaceKHR& surface,
        const vk::SurfaceFormatKHR& surface_format,
        const Device&               device,
        vk::Extent2D                framebuffer_size,
        std::uint32_t               number_of_frames,
        bool                        enable_vsync,
        const Swapchain*            old_swapchain = nullptr
    );

    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::SwapchainKHR&;
    [[nodiscard]]
    auto current_image_index() const noexcept -> std::optional<uint32_t>;
    [[nodiscard]]
    auto current_image_view() const noexcept
        -> util::OptionalRef<const vk::raii::ImageView>;

    [[nodiscard]]
    auto acquire_next_image_index(
        const vk::raii::Semaphore&               semaphore,
        util::OptionalRef<const vk::raii::Fence> fence
    ) -> bool;

private:
    vk::raii::SwapchainKHR           m_swapchain;
    std::vector<vk::Image>           m_swapchain_images;
    std::vector<vk::raii::ImageView> m_swapchain_image_views;
    std::optional<uint32_t>          m_current_image_index;
    bool                             m_out_dated{};
};

}   // namespace kiln::gfx::renderer
