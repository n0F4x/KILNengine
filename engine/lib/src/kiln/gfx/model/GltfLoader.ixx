module;

#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>

export module kiln.gfx.model.GltfLoader;

import kiln.gfx.model.GltfAsset;
import kiln.res.ResourceManager;

namespace kiln::gfx {

export class GltfLoader {
public:
    [[nodiscard]]
    auto load(const std::filesystem::path& filepath, res::ResourceManager& resource_manager)
        -> std::optional<GltfAsset>;

private:
    fastgltf::Parser m_parser;
};

}   // namespace kiln::gfx
