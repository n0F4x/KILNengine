module;

#include <concepts>
#include <type_traits>

export module kiln.reg.entry_c;

import kiln.reg.BuildableEntryBase;
import kiln.reg.EntryBase;
import kiln.reg.EntryBuilderBase;
import kiln.util.containers.MoveOnlyAny;

namespace kiln::reg {

export template <typename T>
concept entry_c = util::storable_in_any_c<T, util::BasicMoveOnlyAny<0>>
               && std::derived_from<T, EntryBase>
               && (std::default_initializable<T>
                   || std::derived_from<T, internal::BuildableEntryBase>)
               && !std::derived_from<T, EntryBuilderBase>;

export template <typename T>
concept decays_to_entry_c = entry_c<std::decay_t<T>>;

}   // namespace kiln::reg
