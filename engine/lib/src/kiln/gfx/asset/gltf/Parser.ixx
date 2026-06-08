module;

#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>

export module kiln.gfx.asset.gltf.Parser;

import kiln.app.registry.EntryBase;

namespace kiln::gfx::asset::gltf {

export class Parser : public app::EntryBase {
public:
    [[nodiscard]]
    auto load(const std::filesystem::path& filepath, bool generate_indices = false)
        -> std::optional<fastgltf::Asset>;

private:
    fastgltf::Parser m_parser;
};

}   // namespace kiln::gfx::asset::gltf
