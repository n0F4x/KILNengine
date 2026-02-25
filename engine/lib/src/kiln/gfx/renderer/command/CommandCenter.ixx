module;

#include <cassert>
#include <cstdint>
#include <optional>
#include <ranges>
#include <vector>

export module kiln.gfx.renderer.command.CommandCenter;

import vulkan_hpp;

import kiln.gfx.renderer.command.CommandBufferHandle;
import kiln.gfx.renderer.command.CommandPool;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.QueueFamilyIndex;

namespace kiln::gfx::renderer {

export class CommandCenter {
public:
    CommandCenter(
        const vk::raii::Device&  device,
        vulkan::QueueFamilyIndex graphics_queue_family_index,
        uint32_t                 number_of_frames
    );

    auto allocate(const vk::raii::Device& device) -> CommandBufferHandle;

    auto swap_buffers() -> void;

private:
    uint32_t                 m_frame_index{};
    std::vector<CommandPool> m_pools;
};

}   // namespace kiln::gfx::renderer

module :private;

namespace kiln::gfx::renderer {

namespace internal {

[[nodiscard]]
auto create_command_pools(
    const vk::raii::Device&        device,
    const vulkan::QueueFamilyIndex graphics_queue_family_index,
    const uint32_t                 number_of_frames
) -> std::vector<CommandPool>
{
    std::vector<CommandPool> result;
    result.reserve(number_of_frames);

    for (const auto _ : std::views::repeat(std::ignore, number_of_frames))
    {
        result.push_back(CommandPool{ device, graphics_queue_family_index });
    }

    return result;
}

}   // namespace internal

CommandCenter::CommandCenter(
    const vk::raii::Device&        device,
    const vulkan::QueueFamilyIndex graphics_queue_family_index,
    const uint32_t                 number_of_frames
)
    : m_pools{
          internal::create_command_pools(
              device,
              graphics_queue_family_index,
              number_of_frames
          )   //
      }
{
}

auto CommandCenter::allocate(const vk::raii::Device& device) -> CommandBufferHandle
{
    std::optional<CommandBufferHandle> result{};

    for (CommandPool& pool : m_pools)
    {
        const CommandBufferHandle handle = pool.allocate(device);
        assert(!result.has_value() || result == handle);
        result = handle;
    }

    return *result;
}

auto CommandCenter::swap_buffers() -> void
{
    m_pools[m_frame_index].reset();
    m_frame_index = (m_frame_index + 1) % m_pools.size();
}

}   // namespace kiln::gfx::renderer
