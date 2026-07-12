module;

#include <fmt/compile.h>

export module kiln.exec.access_pattern_of;

import kiln.exec.AccessPattern;
import kiln.util.reflection;

namespace kiln::exec {

namespace internal {

using namespace fmt::literals;

template <typename T>
struct AccessPatternOf {
    static_assert(
        false,
        fmt::format(
            "specialization not found for `access_pattern_of<{}>`"_cf,
            util::name_of<T>()
        )
    );
};

}   // namespace internal

export template <typename Accessor_T>
constexpr AccessPattern access_pattern_of{ internal::AccessPatternOf<Accessor_T>::value };

}   // namespace kiln::exec
