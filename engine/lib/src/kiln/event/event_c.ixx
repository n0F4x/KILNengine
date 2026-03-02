export module kiln.event.event_c;

import kiln.util.concepts.naked;

namespace kiln::event {

export template <typename T>
concept event_c = util::naked_c<T>;

}   // namespace kiln::event
