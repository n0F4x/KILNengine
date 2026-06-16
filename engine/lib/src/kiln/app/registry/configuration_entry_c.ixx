module;

#include <concepts>

export module kiln.app.registry.configuration_entry_c;

import kiln.app.registry.entry_c;
import kiln.app.registry.BuildableEntryBase;
import kiln.app.registry.ConfigurationEntry;

namespace kiln::app {

export template <typename T>
concept configuration_entry_c = entry_c<T>
                             && std::derived_from<T, ConfigurationEntry>
                             && std::default_initializable<T>
                             && !std::derived_from<T, internal::BuildableEntryBase>;

export template <typename T>
concept decays_to_configuration_entry_c = configuration_entry_c<std::decay_t<T>>;

}   // namespace kiln::app
