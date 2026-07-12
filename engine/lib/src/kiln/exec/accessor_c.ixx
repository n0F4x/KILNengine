module;

#include <concepts>
#include <type_traits>

export module kiln.exec.accessor_c;

import kiln.reg.Registry;
import kiln.exec.AccessPattern;
import kiln.exec.access_pattern_of;
import kiln.util.concepts.naked;
import kiln.util.concepts.storable;

namespace kiln::exec {

export template <typename T>
concept accessor_c
    = util::naked_c<T> && util::storable_c<T> && requires(reg::Registry& registry) {
          access_pattern_of<T>;
          { provide_accessor(std::type_identity<T>{}, registry) } -> std::same_as<T>;
      };

export template <typename T>
concept decays_to_accessor_c = accessor_c<std::remove_cvref_t<T>>;

}   // namespace kiln::exec
