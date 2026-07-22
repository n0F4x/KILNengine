module;

#include <concepts>

export module kiln.reg.describes_entry_build_c;

import kiln.reg.BuildDirector;
import kiln.reg.entry_c;

namespace kiln::reg {

export template <typename T, typename Entry_T>
concept describes_entry_build_c
    = entry_c<Entry_T> && std::invocable<T, BuildDirector<Entry_T>&>;

}   // namespace kiln::reg
