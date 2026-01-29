#pragma once

#include "kiln/util/GenericStack.hpp"

namespace kiln::app {

using ResourceStack = util::GenericStack;

template <typename T>
concept resource_c = util::generic_stack_item_c<T>;

template <typename T>
concept decays_to_resource_c = resource_c<std::decay_t<T>>;

template <typename T>
concept resource_injection_c = util::generic_stack_item_injection_c<T>;

template <typename T>
concept decays_to_resource_injection_c = resource_injection_c<std::decay_t<T>>;

}   // namespace kiln::app
