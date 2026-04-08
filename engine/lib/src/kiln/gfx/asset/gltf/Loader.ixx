module;

#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>

export module kiln.gfx.asset.gltf.Loader;

import kiln.gfx.asset.gltf.Asset;
import kiln.res.ResourceManager;

namespace kiln::gfx::asset::gltf {

export class Loader {
public:
    [[nodiscard]]
    auto load(const std::filesystem::path& filepath, res::ResourceManager& resource_manager)
        -> std::optional<Asset>;

private:
    fastgltf::Parser m_parser;
};

}   // namespace kiln::gfx
