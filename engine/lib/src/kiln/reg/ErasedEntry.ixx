export module kiln.reg.ErasedEntry;

import kiln.reg.entry_c;
import kiln.util.containers.MoveOnlyAny;

namespace kiln::reg {

template <typename T>
struct EntryConcept {
    constexpr static bool value{ entry_c<T> };
};

export using ErasedEntry
    = util::BasicMoveOnlyAny<0, util::default_any_alignment(), EntryConcept>;

}   // namespace kiln::reg
