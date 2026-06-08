export module kiln.app.registry.ErasedEntry;

import kiln.app.registry.entry_c;
import kiln.util.containers.MoveOnlyAny;

namespace kiln::app {

template <typename T>
struct EntryConcept {
    constexpr static bool value{ entry_c<T> };
};

export using ErasedEntry
    = util::BasicMoveOnlyAny<0, util::default_any_alignment(), EntryConcept>;

}   // namespace kiln::app
