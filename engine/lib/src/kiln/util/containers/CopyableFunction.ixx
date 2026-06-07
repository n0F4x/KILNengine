module;

#include <cstddef>

export module kiln.util.containers.CopyableFunction;

import kiln.util.concepts.function;
import kiln.util.containers.Function;

namespace kiln::util {

export template <
    function_c  Signature_T,
    std::size_t size_T      = 3 * sizeof(void*),
    std::size_t alignment_T = sizeof(void*)>
using CopyableFunction = Function<Signature_T, false, size_T, alignment_T>;

export using util::any_cast;

}   // namespace kiln::util
