module;

#include <concepts>
#include <type_traits>

export module kiln.app.registry.entry_c;

import kiln.app.registry.BuildableEntryBase;
import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderBase;
import kiln.util.containers.GenericStack;

namespace kiln::app {

export template <typename T>
concept entry_c =   //
    std::derived_from<T, EntryBase>
    && util::generic_stack_item_c<T>
    && (std::default_initializable<T>
        || std::derived_from<T, internal::BuildableEntryBase>)
    && !std::derived_from<T, EntryBuilderBase>;

export template <typename T>
concept decays_to_entry_c = entry_c<std::decay_t<T>>;

}   // namespace kiln::app
