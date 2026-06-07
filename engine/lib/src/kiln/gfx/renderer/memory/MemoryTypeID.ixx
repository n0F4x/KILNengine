module;

#include <cstdint>
#include <limits>

export module kiln.gfx.renderer.memory.MemoryTypeID;

import kiln.util.Strong;

namespace kiln::gfx::renderer {

export class MemoryTypeID : public util::Strong<uint32_t, MemoryTypeID> {
public:
    [[nodiscard]]
    consteval static auto invalid_value() -> MemoryTypeID;


    constexpr explicit MemoryTypeID();
    constexpr explicit MemoryTypeID(Underlying underlying);
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

constexpr MemoryTypeID::MemoryTypeID(const Underlying underlying) : Strong{ underlying }
{
}

consteval auto MemoryTypeID::invalid_value() -> MemoryTypeID
{
    return MemoryTypeID{ std::numeric_limits<Underlying>::max() };
}

constexpr MemoryTypeID::MemoryTypeID() : MemoryTypeID{ invalid_value() } {}

}   // namespace kiln::gfx::renderer
