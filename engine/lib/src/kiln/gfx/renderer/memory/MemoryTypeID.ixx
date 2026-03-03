module;

#include <cstdint>
#include <limits>

export module kiln.gfx.renderer.memory.MemoryTypeID;

import kiln.util.Strong;

namespace kiln::gfx::renderer {

export class MemoryTypeID : public util::Strong<uint32_t, MemoryTypeID> {
public:
    consteval static auto invalid_value() -> MemoryTypeID
    {
        return MemoryTypeID{ std::numeric_limits<MemoryTypeID>::max() };
    }

    using Strong::Strong;
};

}   // namespace kiln::gfx::renderer
