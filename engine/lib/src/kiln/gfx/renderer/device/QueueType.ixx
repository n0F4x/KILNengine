module;

#include <cstdint>

export module kiln.gfx.renderer.device.QueueType;

import kiln.util.EnumMask;

namespace kiln::gfx::renderer {

export enum struct QueueType : uint32_t
{
    eGraphics             = 1 << 0,
    eHostToDeviceTransfer = 1 << 1,
};

}   // namespace kiln::gfx::renderer

template <>
constexpr bool kiln::util::enable_enum_mask<kiln::gfx::renderer::QueueType> = true;
