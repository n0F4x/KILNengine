module;

#include <chrono>

export module kiln.event.Timestamp;

namespace kiln::event {

export using Timestamp = std::chrono::steady_clock::time_point;

}   // namespace kiln::event
