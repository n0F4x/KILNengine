module;

#include <cstdint>

export module kiln.gfx.renderer.command.CommandBufferUsageFlags;

namespace kiln::gfx::renderer {

export enum struct CommandBufferUsageFlags : uint8_t
{
    eNone       = 0,
    eResettable = 1,
};

}   // namespace kiln::gfx::renderer
