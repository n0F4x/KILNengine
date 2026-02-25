module;

#include <cstdint>

export module kiln.gfx.renderer.command.CommandBufferHandle;

import kiln.util.Strong;

namespace kiln::gfx::renderer {

export class CommandBufferHandle : public util::Strong<uint32_t, CommandBufferHandle> {
public:
    using Strong::Strong;
};

}   // namespace kiln::gfx::renderer
