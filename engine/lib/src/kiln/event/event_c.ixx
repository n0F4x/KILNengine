module;

#include <concepts>

export module kiln.event.event_c;

import kiln.util.concepts.storable;
import kiln.util.concepts.decayed;

namespace kiln::event {

export template <typename T>
concept event_c = util::storable_c<T> && util::decayed_c<T> && std::movable<T>;

}   // namespace kiln::event
