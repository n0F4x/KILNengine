module;

#include <cstddef>
#include <span>

export module kiln.gfx.renderer.stream.LazyCopy;

import kiln.util.containers.MoveOnlyFunction;

namespace kiln::gfx::renderer {

export using LazyCopy = util::MoveOnlyFunction<auto(std::span<std::byte>) &&->void>;

}   // namespace kiln::gfx::renderer
