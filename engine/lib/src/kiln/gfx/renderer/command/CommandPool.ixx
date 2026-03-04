module;

#include <cstdint>
#include <functional>
#include <optional>
#include <ranges>
#include <vector>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.command.CommandPool;

import vulkan_hpp;

import kiln.gfx.renderer.command.GraphicsCommandBuffer;
import kiln.gfx.renderer.command.OneTimeTransferCommandBuffer;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class CommandPool {
public:
    CommandPool(
        [[kiln_lifetimebound]]
        const Device& device,
        uint32_t      number_of_frames
    );

    auto allocate_for_graphics() -> GraphicsCommandBuffer;
    auto allocate_for_one_time_host_to_device_transfer() -> OneTimeTransferCommandBuffer;

    auto swap_buffers() -> void;

private:
    std::reference_wrapper<const Device> m_device;
    uint32_t                             m_frame_index{};

    std::vector<vk::raii::CommandPool>   m_graphics_pools;
    std::vector<vk::raii::CommandBuffer> m_graphics_command_buffers;

    vk::raii::CommandPool m_one_time_host_to_device_transfer_pool;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

namespace internal {

[[nodiscard]]
auto create_graphics_command_pools(const Device& device, const uint32_t number_of_frames)
    -> std::vector<vk::raii::CommandPool>
{
    std::vector<vk::raii::CommandPool> result;

    for (const vk::CommandPoolCreateInfo create_info{
             .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer
                    | vk::CommandPoolCreateFlagBits::eTransient,
             .queueFamilyIndex = device.graphics_queue().family_index().underlying() };
         const auto _ : std::views::repeat(std::ignore, number_of_frames))
    {
        result.push_back(
            vulkan::check_result(device.logical_device().createCommandPool(create_info))
        );
    }

    return result;
}

}   // namespace internal

CommandPool::CommandPool(const Device& device, const uint32_t number_of_frames)
    : m_device{ device },
      m_graphics_pools{
          internal::create_graphics_command_pools(
              device,
              number_of_frames
          )   //
      },
      m_one_time_host_to_device_transfer_pool{
          vulkan::check_result(device.logical_device().createCommandPool(
              vk::CommandPoolCreateInfo{
                  .flags = vk::CommandPoolCreateFlagBits::eTransient,
                  .queueFamilyIndex =
                      device.host_to_device_transfer_queue().family_index().underlying(),
              }
          ))   //
      }
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandPool::allocate_for_graphics() -> GraphicsCommandBuffer
{
    std::vector<vk::raii::CommandBuffer> command_buffers;
    command_buffers.reserve(m_graphics_pools.size());

    for (const vk::raii::CommandPool& pool : m_graphics_pools)
    {
        const vk::CommandBufferAllocateInfo allocate_info{
            .commandPool        = pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1,
        };

        command_buffers.push_back(
            std::move(
                vulkan::check_result(
                    m_device.get().logical_device().allocateCommandBuffers(allocate_info)
                )
                    .front()
            )
        );
    }

    return GraphicsCommandBuffer{ std::move(command_buffers),
                                  static_cast<uint8_t>(m_graphics_pools.size()) };
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto CommandPool::allocate_for_one_time_host_to_device_transfer()
    -> OneTimeTransferCommandBuffer
{
    const vk::CommandBufferAllocateInfo allocate_info{
        .commandPool        = m_one_time_host_to_device_transfer_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    return OneTimeTransferCommandBuffer{
        std::move(
            vulkan::check_result(
                m_device.get().logical_device().allocateCommandBuffers(allocate_info)
            )
                .front()
        )   //
    };
}

auto CommandPool::swap_buffers() -> void
{
    m_graphics_pools[m_frame_index].reset();
    m_frame_index = (m_frame_index + 1) % m_graphics_pools.size();
}

}   // namespace kiln::gfx::renderer
