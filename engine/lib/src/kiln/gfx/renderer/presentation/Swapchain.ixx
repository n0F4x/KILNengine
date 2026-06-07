module;

#include <cstdint>
#include <optional>
#include <span>
#include <vector>

export module kiln.gfx.renderer.presentation.Swapchain;

import vulkan_hpp;

import kiln.gfx.renderer.command.PresentQueueRef;
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
    auto extent() const noexcept -> vk::Extent2D;
    [[nodiscard]]
    auto number_of_images() const noexcept -> uint32_t;
    [[nodiscard]]
    auto image_at(uint32_t index) const noexcept -> const vk::Image&;
    [[nodiscard]]
    auto image_view_at(uint32_t index) const noexcept -> const vk::raii::ImageView&;

    [[nodiscard]]
    auto acquire_next_image_index(
        util::OptionalRef<const vk::raii::Semaphore> signal_semaphore,
        util::OptionalRef<const vk::raii::Fence>     fence
    ) -> std::optional<uint32_t>;

    auto present(
        const PresentQueueRef&         queue,
        uint32_t                       image_index,
        std::span<const vk::Semaphore> wait_semaphores
    ) -> bool;

private:
    vk::SurfaceCapabilitiesKHR       m_surface_capabilities;
    vk::Extent2D                     m_extent;
    vk::raii::SwapchainKHR           m_swapchain;
    std::vector<vk::Image>           m_swapchain_images;
    std::vector<vk::raii::ImageView> m_swapchain_image_views;
    bool                             m_out_dated{};
};

}   // namespace kiln::gfx::renderer
