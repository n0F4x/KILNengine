module;

export module kiln.util.concepts.unambiguous_functor;

import kiln.util.concepts.unambiguous_explicit_functor;
import kiln.util.concepts.unambiguous_implicit_functor;
import kiln.util.concepts.unambiguous_static_functor;

namespace kiln::util {

export template <typename T>
concept unambiguous_functor_c = unambiguous_explicit_functor_c<T>
                             || unambiguous_implicit_functor_c<T>
                             || unambiguous_static_functor_c<T>;

}   // namespace kiln::util
