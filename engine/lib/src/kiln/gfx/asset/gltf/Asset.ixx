module;

#include <fastgltf/core.hpp>

export module kiln.gfx.asset.gltf.Asset;

namespace kiln::gfx::asset::gltf {

export class Asset {
public:
    explicit Asset(fastgltf::Asset&& asset);

    [[nodiscard]]
    auto buffers();

private:
    fastgltf::Asset m_asset;
};

}   // namespace kiln::gfx::asset::gltf
