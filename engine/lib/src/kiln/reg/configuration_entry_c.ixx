module;

#include <concepts>

export module kiln.reg.configuration_entry_c;

import kiln.reg.entry_c;
import kiln.reg.BuildableEntryBase;
import kiln.reg.ConfigurationEntry;

namespace kiln::reg {

export template <typename T>
concept configuration_entry_c = entry_c<T>
                             && std::derived_from<T, ConfigurationEntry>
                             && std::default_initializable<T>
                             && !std::derived_from<T, internal::BuildableEntryBase>;

export template <typename T>
concept decays_to_configuration_entry_c = configuration_entry_c<std::decay_t<T>>;

}   // namespace kiln::reg
