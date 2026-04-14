module;

#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>

export module kiln.gfx.asset.gltf.Parser;

namespace kiln::gfx::asset::gltf {

export class Parser {
public:
    [[nodiscard]]
    auto load(const std::filesystem::path& filepath) -> std::optional<fastgltf::Asset>;

private:
    fastgltf::Parser m_parser;
};

}   // namespace kiln::gfx::asset::gltf
