module;

#include <algorithm>
#include <cstdint>
#include <span>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.vulkan.Swapchain;

import vulkan_hpp;

import kiln.gfx.vulkan.Device;
import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;

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

private:
    vk::SurfaceFormatKHR             m_surface_format;
    vk::raii::SwapchainKHR           m_swapchain;
    std::vector<vk::Image>           m_swapchain_images;
    std::vector<vk::raii::ImageView> m_swapchain_image_views;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

namespace internal {

[[nodiscard]]
auto pick_surface_format(const std::vector<vk::SurfaceFormatKHR>& surface_formats)
    -> vk::SurfaceFormatKHR
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

}   // namespace internal

constexpr auto Swapchain::pick_present_mode(
    const std::span<const vk::PresentModeKHR> present_modes
) -> vk::PresentModeKHR
{
    return std::ranges::contains(present_modes, vk::PresentModeKHR::eMailbox)
             ? vk::PresentModeKHR::eMailbox
         : std::ranges::contains(present_modes, vk::PresentModeKHR::eFifoRelaxed)
             ? vk::PresentModeKHR::eFifoRelaxed
             : vk::PresentModeKHR::eFifo;
}

namespace internal {

[[nodiscard]]
auto pick_swap_extent(
    const vk::Extent2D&               framebuffer_size,
    const vk::SurfaceCapabilitiesKHR& surface_capabilities
) -> vk::Extent2D
{
    if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surface_capabilities.currentExtent;
    }

    return vk::Extent2D{
        .width = std::clamp(
            static_cast<uint32_t>(framebuffer_size.width),
            surface_capabilities.minImageExtent.width,
            surface_capabilities.maxImageExtent.width
        ),
        .height = std::clamp(
            static_cast<uint32_t>(framebuffer_size.height),
            surface_capabilities.minImageExtent.height,
            surface_capabilities.maxImageExtent.height
        ),
    };
}

template <present_mode_picker_c PickPresentMode_T>
[[nodiscard]]
auto create_swapchain(
    const vk::Extent2D&             framebuffer_size,
    const vk::raii::SurfaceKHR&     surface,
    const vk::raii::PhysicalDevice& physical_device,
    const vk::raii::Device&         logical_device,
    const vk::SurfaceFormatKHR&     surface_format,
    PickPresentMode_T&&             pick_present_mode,
    const uint32_t                  number_of_frames
) -> vk::raii::SwapchainKHR
{
    PRECOND(
        logical_device.getDispatcher()->vkCreateSwapchainKHR != nullptr,
        "VK_KHR_SWAPCHAIN_EXTENSION_NAME was not enabled"
    );

    const vk::SurfaceCapabilitiesKHR surface_capabilities{
        physical_device.getSurfaceCapabilitiesKHR(surface)
    };
    const vk::PresentModeKHR present_mode{
        std::invoke(
            std::forward<PickPresentMode_T>(pick_present_mode),
            physical_device.getSurfacePresentModesKHR(surface)
        )   //
    };
    const uint32_t image_count{
        surface_capabilities.maxImageCount == 0
            ? std::max(number_of_frames, surface_capabilities.minImageCount)
            : std::min(
                  std::max(number_of_frames, surface_capabilities.minImageCount),
                  surface_capabilities.maxImageCount
              )   //
    };

    const vk::SwapchainCreateInfoKHR create_info{
        .surface          = surface,
        .minImageCount    = image_count,
        .imageFormat      = surface_format.format,
        .imageColorSpace  = surface_format.colorSpace,
        .imageExtent      = pick_swap_extent(framebuffer_size, surface_capabilities),
        .imageArrayLayers = 1,
        .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform     = surface_capabilities.currentTransform,
        .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode      = present_mode,
        .clipped          = vk::True,
    };

    return check_result(logical_device.createSwapchainKHR(create_info));
}

[[nodiscard]]
auto create_swapchain_image_views(
    const vk::raii::Device&          device,
    const vk::Format                 swapchain_image_format,
    const std::span<const vk::Image> swapchain_images
) -> std::vector<vk::raii::ImageView>
{
    std::vector<vk::raii::ImageView> result;
    result.reserve(swapchain_images.size());

    constexpr static vk::ImageSubresourceRange subresource_range{
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };
    vk::ImageViewCreateInfo create_info{
        .viewType         = vk::ImageViewType::e2D,
        .format           = swapchain_image_format,
        .subresourceRange = subresource_range,
    };

    for (const vk::Image swapchain_image : swapchain_images)
    {
        create_info.image = swapchain_image;
        result.push_back(check_result(device.createImageView(create_info)));
    }

    return result;
}

}   // namespace internal

Swapchain::Swapchain(
    const vk::raii::SurfaceKHR&     surface,
    const vk::raii::PhysicalDevice& physical_device,
    const vk::raii::Device&         logical_device,
    const vk::Extent2D              framebuffer_size,
    const uint32_t                  number_of_frames
)
    : Swapchain{
          surface,            //
          physical_device,    //
          logical_device,     //
          framebuffer_size,   //
          number_of_frames,   //
          pick_present_mode   //
      }
{
}

template <present_mode_picker_c PickPresentMode_T>
Swapchain::Swapchain(
    const vk::raii::SurfaceKHR&     surface,
    const vk::raii::PhysicalDevice& physical_device,
    const vk::raii::Device&         logical_device,
    vk::Extent2D                    framebuffer_size,
    uint32_t                        number_of_frames,
    PickPresentMode_T&&             pick_present_mode
)
    : m_surface_format{
          internal::pick_surface_format(physical_device.getSurfaceFormatsKHR(surface))   //
      },
      m_swapchain{
          internal::create_swapchain(
              framebuffer_size,
              surface,
              physical_device,
              logical_device,
              m_surface_format,
              std::forward<PickPresentMode_T>(pick_present_mode),
              number_of_frames
          )   //
      },
      m_swapchain_images{ m_swapchain.getImages() },
      m_swapchain_image_views{
          internal::create_swapchain_image_views(
              logical_device,
              m_surface_format.format,
              m_swapchain_images
          )   //
      }
{
}

auto Swapchain::surface_format() const noexcept -> vk::SurfaceFormatKHR
{
    return m_surface_format;
}

}   // namespace kiln::gfx::vulkan
