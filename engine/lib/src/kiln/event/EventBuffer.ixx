module;

#include <chrono>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <vector>

export module kiln.event.EventBuffer;

import kiln.event.event_c;

namespace kiln::event {

export template <event_c Event_T>
class EventBuffer {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    EventBuffer(const EventBuffer&, const allocator_type&)
        requires(std::is_copy_constructible_v<Event_T>);
    EventBuffer(EventBuffer&&, const allocator_type&);

    explicit EventBuffer() = default;
    explicit EventBuffer(std::allocator_arg_t, const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;

    auto clear() -> void;
    auto reserve(std::size_t capacity) -> void;
    auto push_back(Event_T&& event, std::chrono::steady_clock::time_point timestamp)
        -> void;

private:
    std::pmr::vector<Event_T>                               m_events;
    std::pmr::vector<std::chrono::steady_clock::time_point> m_timestamps;
};

}   // namespace kiln::event

namespace kiln::event {

template <event_c Event_T>
EventBuffer<Event_T>::EventBuffer(
    const EventBuffer&    other,
    const allocator_type& allocator
)
    requires(std::is_copy_constructible_v<Event_T>)
    : m_events{ other.m_events, allocator },
      m_timestamps{ other.m_timestamps, allocator }
{
}

template <event_c Event_T>
EventBuffer<Event_T>::EventBuffer(EventBuffer&& other, const allocator_type& allocator)
    : m_events{ std::move(other.m_events), allocator },
      m_timestamps{ std::move(other.m_timestamps), allocator }
{
}

template <event_c Event_T>
EventBuffer<Event_T>::EventBuffer(std::allocator_arg_t, const allocator_type& allocator)
    : m_events{ allocator },
      m_timestamps{ allocator }
{
}

template <event_c Event_T>
auto EventBuffer<Event_T>::get_allocator() const noexcept -> allocator_type
{
    return m_events.get_allocator();
}

template <event_c Event_T>
auto EventBuffer<Event_T>::size() const noexcept -> std::size_t
{
    return m_events.size();
}

template <event_c Event_T>
auto EventBuffer<Event_T>::clear() -> void
{
    m_events.clear();
    m_timestamps.clear();
}

template <event_c Event_T>
auto EventBuffer<Event_T>::reserve(const std::size_t capacity) -> void
{
    m_events.reserve(capacity);
    m_timestamps.reserve(capacity);
}

template <event_c Event_T>
auto EventBuffer<Event_T>::push_back(
    Event_T&&                                   event,
    const std::chrono::steady_clock::time_point timestamp
) -> void
{
    m_events.push_back(std::move(event));
    m_timestamps.push_back(timestamp);
}

}   // namespace kiln::event
