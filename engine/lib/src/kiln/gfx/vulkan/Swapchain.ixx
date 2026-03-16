module;

#include <cstdint>
#include <span>
#include <vector>

export module kiln.gfx.vulkan.Swapchain;

import vulkan_hpp;

import kiln.gfx.vulkan.Device;

namespace kiln::gfx::vulkan {

export template <typename T>
concept present_mode_picker_c = requires {
    std::same_as<
        std::invoke_result_t<T&&, std::span<const vk::PresentModeKHR>>,
        vk::PresentModeKHR>;
};

export class Swapchain {
public:
    [[nodiscard]]
    constexpr static auto pick_present_mode(
        std::span<const vk::PresentModeKHR> present_modes
    ) -> vk::PresentModeKHR;

    Swapchain(
        const vk::raii::SurfaceKHR&     surface,
        const vk::raii::PhysicalDevice& physical_device,
        const vk::raii::Device&         logical_device,
        vk::Extent2D                    framebuffer_size,
        uint32_t                        number_of_frames
    );

    template <present_mode_picker_c PickPresentMode_T>
    Swapchain(
        const vk::raii::SurfaceKHR&     surface,
        const vk::raii::PhysicalDevice& physical_device,
        const vk::raii::Device&         logical_device,
        vk::Extent2D                    framebuffer_size,
        uint32_t                        number_of_frames,
        PickPresentMode_T&&             pick_present_mode
    );

    [[nodiscard]]
    auto surface_format() const noexcept -> vk::SurfaceFormatKHR;
    [[nodiscard]]
    auto swapchain_image_views() const noexcept -> std::span<const vk::raii::ImageView>;

private:
    vk::SurfaceFormatKHR             m_surface_format;
    vk::raii::SwapchainKHR           m_swapchain;
    std::vector<vk::Image>           m_swapchain_images;
    std::vector<vk::raii::ImageView> m_swapchain_image_views;
};

}   // namespace kiln::gfx::vulkan
