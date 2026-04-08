module;

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <variant>

export module kiln.gfx.asset.VertexArrayView;

namespace kiln::gfx::asset {

export struct VertexArrayView {
    std::variant<std::filesystem::path, std::span<const std::byte>> bytes;
    uint64_t                                                        byte_length;
    uint64_t                                                        byte_offset;
    uint64_t                                                        byte_stride{};
};

}   // namespace kiln::gfx::asset
