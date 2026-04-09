module;

#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>

export module kiln.gfx.asset.gltf.Parser;

import kiln.gfx.asset.gltf.Asset;

namespace kiln::gfx::asset::gltf {

export class Parser {
public:
    [[nodiscard]]
    auto load(
        const std::filesystem::path& filepath,
        const Asset::allocator_type& allocator = std::pmr::get_default_resource()
    ) -> std::optional<Asset>;

private:
    fastgltf::Parser m_parser;
};

}   // namespace kiln::gfx::asset::gltf
