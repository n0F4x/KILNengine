module;

#include <utility>

#include <fastgltf/core.hpp>

module kiln.gfx.asset.gltf.Asset;

namespace kiln::gfx::asset::gltf {

Asset::Asset(fastgltf::Asset&& asset) : m_asset{ std::move(asset) } {}

auto Asset::buffers()
{
    return m_asset.buffers;
}

}   // namespace kiln::gfx::asset::gltf
