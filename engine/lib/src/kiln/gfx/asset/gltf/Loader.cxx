module;

#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.asset.gltf.Loader;

import kiln.util.contracts;

namespace kiln::gfx::asset::gltf {

auto Loader::load(const std::filesystem::path& filepath, res::ResourceManager&)
    -> std::optional<Asset>
{
    fastgltf::GltfFileStream file{ filepath };
    if (!file.isOpen())
    {
        return std::nullopt;
    }

    fastgltf::Expected<fastgltf::Asset> asset{
        m_parser.loadGltf(file, filepath.parent_path(), fastgltf::Options::None)
    };
    if (asset.error() != fastgltf::Error::None)
    {
        return std::nullopt;
    }

    PRECOND(fastgltf::validate(asset.get()) == fastgltf::Error::None);

    return Asset{ std::move(asset.get()) };
}

}   // namespace kiln::gfx::asset::gltf
