module;

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <variant>

export module kiln.gfx.asset.IndexArrayView;

namespace kiln::gfx::asset {

export struct IndexArrayView {
    std::variant<std::filesystem::path, std::span<const std::byte>> bytes;
    uint64_t                                                        byte_length;
    uint64_t                                                        byte_offset;
};

}   // namespace kiln::gfx::asset
