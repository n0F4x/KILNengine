module;

#include <algorithm>
#include <span>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.swapchain.Swapchain;

import kiln.gfx.vulkan.result.check_result;
import kiln.util.contracts;

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

constexpr auto pick_present_mode(
    const std::span<const vk::PresentModeKHR> present_modes,
    const bool                                vsync
) -> vk::PresentModeKHR
{
    if (vsync && std::ranges::contains(present_modes, vk::PresentModeKHR::eImmediate))
    {
        return vk::PresentModeKHR::eImmediate;
    }

    return std::ranges::contains(present_modes, vk::PresentModeKHR::eMailbox)
             ? vk::PresentModeKHR::eMailbox
         : std::ranges::contains(present_modes, vk::PresentModeKHR::eFifoRelaxed)
             ? vk::PresentModeKHR::eFifoRelaxed
             : vk::PresentModeKHR::eFifo;
}

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

[[nodiscard]]
auto create_swapchain(
    const Device&               device,
    const vk::raii::SurfaceKHR& surface,
    const vk::Extent2D&         framebuffer_size,
    const uint32_t              number_of_frames,
    const bool                  vsync,
    const vk::SurfaceFormatKHR& surface_format
) -> vk::raii::SwapchainKHR
{
    // TODO: remove this PRECOND after
    // https://github.com/KhronosGroup/Vulkan-Hpp/issues/2410
    PRECOND(
        device.logical_device().getDispatcher()->vkCreateSwapchainKHR != nullptr,
        "VK_KHR_SWAPCHAIN_EXTENSION_NAME was not enabled"
    );

    const vk::SurfaceCapabilitiesKHR surface_capabilities{
        device.physical_device().getSurfaceCapabilitiesKHR(surface)
    };
    const vk::PresentModeKHR present_mode{
        pick_present_mode(
            device.physical_device().getSurfacePresentModesKHR(surface), vsync
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

    return vulkan::check_result(device.logical_device().createSwapchainKHR(create_info));
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
        result.push_back(vulkan::check_result(device.createImageView(create_info)));
    }

    return result;
}

Swapchain::Swapchain(
    const Device&               device,
    const vk::raii::SurfaceKHR& surface,
    const vk::Extent2D          framebuffer_size,
    const uint32_t              number_of_frames,
    const bool                  enable_vsync
)
    : m_surface_format{
          pick_surface_format(device.physical_device().getSurfaceFormatsKHR(surface))
      },
      m_swapchain{
          create_swapchain(
              device,
              surface,
              framebuffer_size,
              number_of_frames,
              enable_vsync,
              m_surface_format
          )   //
      },
      m_swapchain_images{ m_swapchain.getImages() },
      m_swapchain_image_views{
          create_swapchain_image_views(
              device.logical_device(),
              m_surface_format.format,
              m_swapchain_images
          )   //
      }
{
}

auto Swapchain::surface_format() const noexcept -> const vk::SurfaceFormatKHR&
{
    return m_surface_format;
}

}   // namespace kiln::gfx::renderer
