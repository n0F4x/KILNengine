module;

#include <concepts>
#include <span>
#include <type_traits>

export module kiln.exec.accessor_c;

import kiln.reg.Registry;
import kiln.exec.Access;
import kiln.util.concepts.naked;
import kiln.util.concepts.storable;

namespace kiln::exec {

export template <typename T>
concept accessor_c
    = util::naked_c<T> && util::storable_c<T> && requires(reg::Registry& registry) {
          {
              accesses_of(std::type_identity<T>{})
          } -> std::same_as<std::span<const Access>>;
          { provide_accessor(std::type_identity<T>{}, registry) } -> std::same_as<T>;
      };

export template <typename T>
concept decays_to_accessor_c = accessor_c<std::remove_cvref_t<T>>;

}   // namespace kiln::exec
