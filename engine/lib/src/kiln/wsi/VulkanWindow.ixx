module;

#include <cassert>
#include <cstdint>
#include <utility>

#include <GLFW/glfw3.h>

export module kiln.wsi.VulkanWindow;

import vulkan_hpp;

import kiln.gfx.vulkan.Device;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.Swapchain;
import kiln.wsi.Context;
import kiln.wsi.Window;

namespace kiln::wsi {

export class VulkanWindow : public Window {
public:
    VulkanWindow(
        Window&&                        window,
        const vk::raii::Instance&       vulkan_instance,
        const vk::raii::PhysicalDevice& physical_device,
        const vk::raii::Device&         logical_device,
        uint32_t                        number_of_frames
    );

    template <gfx::vulkan::present_mode_picker_c PickPresentMode_T>
    VulkanWindow(
        Window&&                        window,
        const vk::raii::Instance&       vulkan_instance,
        const vk::raii::PhysicalDevice& physical_device,
        const vk::raii::Device&         logical_device,
        uint32_t                        number_of_frames,
        PickPresentMode_T&&             pick_present_mode
    );

    [[nodiscard]]
    auto swapchain() const noexcept -> const gfx::vulkan::Swapchain&;

private:
    vk::raii::SurfaceKHR   m_surface;
    gfx::vulkan::Swapchain m_swapchain;
};

}   // namespace kiln::wsi

namespace kiln::wsi {

namespace internal {

[[nodiscard]]
auto create_surface(const Context&, GLFWwindow& window, const vk::raii::Instance& instance)
    -> vk::raii::SurfaceKHR
{
    VkSurfaceKHR surface{};
    gfx::vulkan::check_result(
        glfwCreateWindowSurface(*instance, &window, nullptr, &surface)
    );

    [[maybe_unused]]
    const int error_code = glfwGetError(nullptr);
    assert(
        (error_code != GLFW_API_UNAVAILABLE && error_code != GLFW_INVALID_VALUE
         && error_code != GLFW_PLATFORM_ERROR)
        && "A precondition should have already been violated"
    );
    assert(error_code == GLFW_NO_ERROR);

    return vk::raii::SurfaceKHR{ instance, surface };
}

}   // namespace internal

VulkanWindow::VulkanWindow(
    Window&&                        window,
    const vk::raii::Instance&       vulkan_instance,
    const vk::raii::PhysicalDevice& physical_device,
    const vk::raii::Device&         logical_device,
    const uint32_t                  number_of_frames
)
    : VulkanWindow{
          std::move(window),                          //
          vulkan_instance,                            //
          physical_device,                            //
          logical_device,                             //
          number_of_frames,                           //
          gfx::vulkan::Swapchain::pick_present_mode   //
      }
{
}

template <gfx::vulkan::present_mode_picker_c PickPresentMode_T>
VulkanWindow::VulkanWindow(
    Window&&                        window,
    const vk::raii::Instance&       vulkan_instance,
    const vk::raii::PhysicalDevice& physical_device,
    const vk::raii::Device&         logical_device,
    const uint32_t                  number_of_frames,
    PickPresentMode_T&&             pick_present_mode
)
    : Window{
          std::move(window)
},
      m_surface{ internal::create_surface(context(), handle(), vulkan_instance) },
      m_swapchain{
          m_surface,
          physical_device,
          logical_device,
          vk::Extent2D{
              .width  = resolution().width,
              .height = resolution().height,
          },
          number_of_frames,
          pick_present_mode,
      }
{
}

auto VulkanWindow::swapchain() const noexcept -> const gfx::vulkan::Swapchain&
{
    return m_swapchain;
}

}   // namespace kiln::wsi
