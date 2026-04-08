module;

#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>

module kiln.gfx.model.GltfLoader;

namespace kiln::gfx {

auto GltfLoader::load(const std::filesystem::path& filepath, res::ResourceManager&)
    -> std::optional<GltfAsset>
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

    return GltfAsset{ std::move(asset.get()) };
}

}   // namespace kiln::gfx
