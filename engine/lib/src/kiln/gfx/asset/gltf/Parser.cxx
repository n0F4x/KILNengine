module;

#include <filesystem>
#include <memory_resource>
#include <optional>
#include <vector>

#include <fastgltf/core.hpp>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.asset.gltf.Parser;

import kiln.gfx.asset.Buffer;
import kiln.util.contracts;

namespace kiln::gfx::asset::gltf {

auto Parser::load(
    const std::filesystem::path& filepath,
    const Asset::allocator_type& allocator
) -> std::optional<Asset>
{
    fastgltf::GltfFileStream file{ filepath };
    if (!file.isOpen())
    {
        return std::nullopt;
    }

    /*
     * fastgltf loads glb buffer into memory.
     * It's better to provide our own allocation for that.
     */

    std::pmr::vector<Buffer> custom_buffers{ allocator };

    m_parser.setUserPointer(&custom_buffers);
    m_parser.setBufferAllocationCallback(
        +[](const uint64_t buffer_size, void* const user_pointer) -> fastgltf::BufferInfo
        {
            auto& x_custom_buffers{
                *static_cast<std::pmr::vector<Buffer>*>(user_pointer)
            };

            Buffer& buffer = x_custom_buffers.emplace_back(buffer_size);

            return fastgltf::BufferInfo{
                .mappedMemory = buffer.bytes().data(),
                .customId     = x_custom_buffers.size() - 1,
            };
        }
    );

    fastgltf::Expected<fastgltf::Asset> asset{
        m_parser.loadGltf(file, filepath.parent_path(), fastgltf::Options::None)
    };
    if (asset.error() != fastgltf::Error::None)
    {
        return std::nullopt;
    }

    m_parser.setBufferAllocationCallback(nullptr);
    m_parser.setUserPointer(nullptr);

    PRECOND(fastgltf::validate(asset.get()) == fastgltf::Error::None);

    return Asset{
        std::allocator_arg, allocator, std::move(asset.get()), std::move(custom_buffers)
    };
}

}   // namespace kiln::gfx::asset::gltf
