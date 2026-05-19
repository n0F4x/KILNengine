module;

#include <utility>

export module kiln.gfx.renderer.command.CommandPoolFlags;

import vulkan_hpp;

import kiln.util.EnumMask;

namespace kiln::gfx::renderer {

export enum struct CommandPoolFlags : vk::CommandPoolCreateFlags::MaskType
{
    eNone       = 0,
    eResettable = std::to_underlying(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
};

}   // namespace kiln::gfx::renderer

template <>
constexpr bool kiln::util::enable_enum_mask<kiln::gfx::renderer::CommandPoolFlags> = true;
