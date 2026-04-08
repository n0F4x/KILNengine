module;

#include <cstdint>
#include <filesystem>
#include <span>
#include <variant>

export module kiln.gfx.asset.ByteView;

import kiln.gfx.asset.ByteViewType;

namespace kiln::gfx::asset {

export struct ByteView {
    ByteViewType type{ ByteViewType::eUnknown };
    std::variant<std::filesystem::path, std::span<const std::byte>> bytes;
    uint64_t                                                        byte_length;
    uint64_t                                                        byte_offset;
    /*
     * Byte stride is only defined for vertices
     */
    uint32_t                                                        byte_stride{};
};

}   // namespace kiln::gfx::asset
