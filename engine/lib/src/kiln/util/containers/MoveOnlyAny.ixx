module;

#include <cstddef>

export module kiln.util.containers.MoveOnlyAny;

import kiln.util.containers.Any;

namespace kiln::util {

export using util::default_any_size;
export using util::default_any_alignment;
export using util::DefaultAnyPolicy;
export using util::DefaultAnyInterfaceMixin;
export using util::DefaultAnyExtraVTable;

export template <
    std::size_t size_T                                  = default_any_size(),
    std::size_t alignment_T                             = default_any_alignment(),
    template <typename> typename Policy_T               = DefaultAnyPolicy,
    template <typename Any_T> typename InterfaceMixin_T = DefaultAnyInterfaceMixin,
    template <typename Any_T> typename ExtraVTable_T    = DefaultAnyExtraVTable>
using BasicMoveOnlyAny = BasicAny<
    DefaultAnyTraits<true, size_T, alignment_T, Policy_T, InterfaceMixin_T, ExtraVTable_T>>;

export using MoveOnlyAny = BasicMoveOnlyAny<>;

export template <typename T>
concept move_only_any_c = any_c<T> && (T::is_move_only());

export using util::any_cast;

}   // namespace kiln::util
