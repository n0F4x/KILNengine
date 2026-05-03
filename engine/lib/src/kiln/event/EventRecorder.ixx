module;

#include <chrono>
#include <concepts>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <vector>

export module kiln.event.EventRecorder;

import kiln.event.event_c;
import kiln.event.EventBuffer;

namespace kiln::event {

export template <event_c Event_T>
class EventRecorder {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    EventRecorder(const EventRecorder&, const allocator_type&)
        requires(std::is_copy_constructible_v<Event_T>);
    EventRecorder(EventRecorder&&, const allocator_type&);

    explicit EventRecorder() = default;
    explicit EventRecorder(std::allocator_arg_t, const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <typename... Args_T>
        requires std::constructible_from<Event_T, Args_T&&...>
    auto record(Args_T&&... args) -> void;

    auto flush(EventBuffer<Event_T>& event_buffer) -> void;

private:
    std::pmr::vector<Event_T>                               m_events;
    std::pmr::vector<std::chrono::steady_clock::time_point> m_timestamps;
};

}   // namespace kiln::event

namespace kiln::event {

template <event_c Event_T>
EventRecorder<Event_T>::EventRecorder(
    const EventRecorder&  other,
    const allocator_type& allocator
)
    requires(std::is_copy_constructible_v<Event_T>)
    : m_events{ other.m_events, allocator },
      m_timestamps{ other.m_timestamps, allocator }
{
}

template <event_c Event_T>
EventRecorder<Event_T>::EventRecorder(
    EventRecorder&&       other,
    const allocator_type& allocator
)
    : m_events{ std::move(other.m_events), allocator },
      m_timestamps{ std::move(other.m_timestamps), allocator }
{
}

template <event_c Event_T>
EventRecorder<Event_T>::EventRecorder(
    std::allocator_arg_t,
    const allocator_type& allocator
)
    : m_events{ allocator },
      m_timestamps{ allocator }
{
}

template <event_c Event_T>
auto EventRecorder<Event_T>::get_allocator() const noexcept -> allocator_type
{
    return m_events.get_allocator();
}

template <event_c Event_T>
template <typename... Args_T>
    requires std::constructible_from<Event_T, Args_T&&...>
auto EventRecorder<Event_T>::record(Args_T&&... args) -> void
{
    const auto timestamp{ std::chrono::steady_clock::now() };
    m_events.emplace_back(std::forward<Args_T>(args)...);
    m_timestamps.push_back(timestamp);
}

template <event_c Event_T>
auto EventRecorder<Event_T>::flush(EventBuffer<Event_T>& event_buffer) -> void
{
    event_buffer.reserve(event_buffer.size() + m_events.size());

    for (std::size_t index{}; index < m_events.size(); ++index)
    {
        event_buffer.push_back(std::move(m_events[index], m_timestamps[index]));
    }
}

}   // namespace kiln::event
