module;

#include <cstddef>
#include <span>

export module kiln.gfx.renderer.memory.StreamingService;

import vulkan_hpp;

import kiln.gfx.renderer.command.TransferCommandBuffer;
import kiln.gfx.renderer.memory.Allocator;
import kiln.gfx.renderer.memory.Buffer;

namespace kiln::gfx::renderer {

export class StreamingService {
public:
    static auto stage(
        Allocator&                 allocator,
        std::span<const std::byte> data,
        Buffer&                    destination,
        TransferCommandBuffer&     command_buffer
    ) -> void;
};

}   // namespace kiln::gfx::renderer
