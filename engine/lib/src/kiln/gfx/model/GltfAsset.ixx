module;

#include <fastgltf/core.hpp>

export module kiln.gfx.model.GltfAsset;

namespace kiln::gfx {

export class GltfAsset {
public:
    explicit GltfAsset(fastgltf::Asset&& asset);

private:
    fastgltf::Asset m_asset;
};

}   // namespace kiln::gfx
