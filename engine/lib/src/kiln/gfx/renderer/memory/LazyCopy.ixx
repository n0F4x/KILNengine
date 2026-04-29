module;

#include <cstddef>
#include <span>

export module kiln.gfx.renderer.memory.LazyCopy;

import kiln.util.containers.MoveOnlyFunction;

namespace kiln::gfx::renderer {

export using LazyCopy = util::MoveOnlyFunction<auto(std::span<std::byte>) const->void>;

}   // namespace kiln::gfx::renderer
