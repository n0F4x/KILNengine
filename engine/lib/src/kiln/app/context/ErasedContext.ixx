export module kiln.app.context.ErasedContext;

import kiln.app.context.context_c;
import kiln.util.containers.MoveOnlyAny;

namespace kiln::app {

template <typename T>
struct ContextConcept {
    constexpr static bool value{ context_c<T> };
};

export using ErasedContext
    = util::BasicMoveOnlyAny<0, util::default_any_alignment(), ContextConcept>;

}   // namespace kiln::app
