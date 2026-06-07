export module kiln.util.concepts.nothrow_move_constructible;

import kiln.util.concepts.nothrow_constructible_from;
import kiln.util.concepts.nothrow_convertible_to;

namespace kiln::util {

export template <typename T>
concept nothrow_move_constructible_c
    = nothrow_constructible_from_c<T, T> && nothrow_convertible_to_c<T, T>;

}   // namespace kiln::util
