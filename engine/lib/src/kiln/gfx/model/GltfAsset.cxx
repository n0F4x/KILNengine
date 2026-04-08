module;

#include <utility>

#include <fastgltf/core.hpp>

module kiln.gfx.model.GltfAsset;

namespace kiln::gfx {

GltfAsset::GltfAsset(fastgltf::Asset&& asset) : m_asset{ std::move(asset) } {}

}   // namespace kiln::gfx
