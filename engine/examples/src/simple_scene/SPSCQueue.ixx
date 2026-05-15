module;

#include <atomic>
#include <bit>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <span>
#include <type_traits>

#include "kiln/util/contract_macros.hpp"

export module examples.simple_scene.SPSCQueue;

import kiln.util.concepts.storable;
import kiln.util.contracts;
import kiln.util.ScopeFail;

namespace demo {

template <typename T>
class split_span_view : public std::ranges::view_base {
public:
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept  = std::random_access_iterator_tag;
        using value_type        = std::remove_const_t<T>;
        using reference         = T&;
        using difference_type   = std::ptrdiff_t;

        iterator() = default;

        [[nodiscard]]
        auto operator*() const -> reference
        {
            PRECOND(m_first != nullptr && m_second != nullptr);
            return m_index < m_first_size ? (*m_first)[m_index]
                                          : (*m_second)[m_index - m_first_size];
        }

        [[nodiscard]]
        auto operator==(const iterator& rhs) const noexcept -> bool
        {
            return m_index == rhs.m_index;
        }

        [[nodiscard]]
        auto operator<=>(const iterator& rhs) const noexcept
        {
            return m_index <=> rhs.m_index;
        }

        auto operator++() noexcept -> iterator&
        {
            ++m_index;
            return *this;
        }

        [[nodiscard("use pre-increment if you don't need the old value")]]
        auto operator++(int) noexcept -> iterator
        {
            const auto tmp{ *this };
            ++*this;
            return tmp;
        }

        auto operator--() noexcept -> iterator&
        {
            --m_index;
            return *this;
        }

        [[nodiscard("use pre-decrement if you don't need the old value")]]
        auto operator--(int) noexcept -> iterator
        {
            const auto tmp{ *this };
            --*this;
            return tmp;
        }

        [[nodiscard]]
        auto operator+(const difference_type difference) const noexcept -> iterator
        {
            return iterator{ m_first, m_second, m_first_size, m_index + difference };
        }

        [[nodiscard]]
        friend auto operator+(
            const difference_type difference,
            const iterator        iter
        ) noexcept -> iterator
        {
            return iter + difference;
        }

        auto operator+=(const difference_type difference) noexcept -> iterator&
        {
            m_index += difference;
            return *this;
        }

        [[nodiscard]]
        auto operator-(const iterator other) const noexcept -> difference_type
        {
            return static_cast<difference_type>(m_index) - other.m_index;
        }

        [[nodiscard]]
        auto operator-(const difference_type difference) const -> iterator
        {
            PRECOND(m_index >= difference);
            return iterator{ m_first, m_second, m_first_size, m_index - difference };
        }

        auto operator-=(const difference_type difference) -> iterator&
        {
            PRECOND(m_index >= difference);
            m_index -= difference;
            return *this;
        }

        [[nodiscard]]
        auto operator[](const difference_type difference) const -> reference
        {
            return *operator+(difference);
        }

    private:
        friend class split_span_view;

        explicit iterator(
            const std::span<T>* first,
            const std::span<T>* second,
            const std::size_t   first_size,
            const std::size_t   index
        ) noexcept
            : m_first{ first },
              m_second{ second },
              m_first_size{ first_size },
              m_index{ index }
        {
        }

        const std::span<T>* m_first{};
        const std::span<T>* m_second{};
        std::size_t         m_first_size{};
        std::size_t         m_index{};
    };

    explicit split_span_view(std::span<T> first, std::span<T> second) noexcept
        : m_first{ first },
          m_second{ second }
    {
    }

    [[nodiscard]]
    auto begin() const noexcept -> iterator
    {
        return iterator{ &m_first, &m_second, m_first.size(), 0uz };
    }

    [[nodiscard]]
    auto end() const noexcept -> iterator
    {
        return iterator{
            &m_first,
            &m_second,
            m_first.size(),
            m_first.size() + m_second.size(),
        };
    }

    [[nodiscard]]
    auto size() const noexcept -> std::size_t
    {
        return m_first.size() + m_second.size();
    }

private:
    std::span<T> m_first;
    std::span<T> m_second;
};

static_assert(std::ranges::view<split_span_view<int>>);
static_assert(std::ranges::random_access_range<split_span_view<int>>);
static_assert(std::ranges::sized_range<split_span_view<int>>);
static_assert(std::ranges::common_range<split_span_view<int>>);

class SPSCQueuePrecondition {
public:
    explicit SPSCQueuePrecondition(const std::size_t capacity)
    {
        PRECOND(std::has_single_bit(capacity), "capacity must be a power of 2");
    }
};

export template <kiln::util::storable_c T>
class SPSCQueue : SPSCQueuePrecondition {
public:
    using value_type     = T;
    using allocator_type = std::pmr::polymorphic_allocator<>;


    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue(SPSCQueue&&)      = delete;
    ~SPSCQueue();

    explicit SPSCQueue(std::size_t capacity);
    explicit SPSCQueue(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::size_t           capacity
    );

    SPSCQueue& operator=(const SPSCQueue&) = delete;
    SPSCQueue& operator=(SPSCQueue&&)      = delete;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto capacity() const noexcept -> std::size_t;

    [[nodiscard]]
    auto try_push(const T& value) -> bool
        requires std::copy_constructible<T>;
    [[nodiscard]]
    auto try_push(T&& value) -> bool
        requires std::move_constructible<T>;
    template <typename... Args_T>
        requires std::constructible_from<T, Args_T&&...>
    [[nodiscard]]
    auto try_emplace(Args_T&&... args) -> bool;
    template <std::ranges::input_range R>
        requires std::constructible_from<T, std::ranges::range_reference_t<R>>
    [[nodiscard]]
    auto try_append_range(R&& values) -> std::size_t;

    [[nodiscard]]
    auto pop() noexcept(std::is_nothrow_move_constructible_v<T>) -> std::optional<T>
        requires std::move_constructible<T>;
    template <typename F>
        requires std::invocable<F, std::ranges::as_rvalue_view<std::span<T>>>
              && std::invocable<F, std::ranges::as_rvalue_view<split_span_view<T>>>
    auto pop_all(F&& callback) noexcept(
        std::is_nothrow_invocable_v<F, std::ranges::as_rvalue_view<std::span<T>>>
        && std::is_nothrow_invocable_v<F, std::ranges::as_rvalue_view<split_span_view<T>>>
    ) -> std::size_t;

private:
    allocator_type m_allocator;
    std::size_t    m_capacity;
    std::size_t    m_mask;
    T*             m_data;
    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> m_tail{};
    std::size_t m_head_cache{};
    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> m_head{};
    std::size_t m_tail_cache{};


    [[nodiscard]]
    auto at(std::size_t index) noexcept -> T&;
};

}   // namespace demo

namespace demo {

template <kiln::util::storable_c T>
SPSCQueue<T>::~SPSCQueue()
{
    if constexpr (!std::is_trivially_destructible_v<T>)
    {
        const std::size_t head{ m_head.load(std::memory_order_relaxed) };
        const std::size_t tail{ m_tail.load(std::memory_order_relaxed) };
        for (std::size_t i{ head }; i != tail; ++i)
        {
            std::destroy_at(std::addressof(at(i)));
        }
    }
    m_allocator.deallocate_object(m_data, m_capacity);
}

template <kiln::util::storable_c T>
SPSCQueue<T>::SPSCQueue(const std::size_t capacity)

    : SPSCQueue{ std::allocator_arg, std::pmr::get_default_resource(), capacity }
{
}

template <kiln::util::storable_c T>
SPSCQueue<T>::SPSCQueue(
    std::allocator_arg_t,
    const allocator_type& allocator,
    const std::size_t     capacity
)
    : SPSCQueuePrecondition{ capacity },
      m_allocator{ allocator },
      m_capacity{ capacity },
      m_mask{ m_capacity - 1uz },
      m_data{ m_allocator.allocate_object<T>(m_capacity) }
{
}

template <kiln::util::storable_c T>
auto SPSCQueue<T>::get_allocator() const noexcept -> allocator_type
{
    return allocator_type{ m_allocator };
}

template <kiln::util::storable_c T>
auto SPSCQueue<T>::capacity() const noexcept -> std::size_t
{
    return m_capacity;
}

template <kiln::util::storable_c T>
auto SPSCQueue<T>::try_push(const T& value) -> bool
    requires std::copy_constructible<T>
{
    return try_emplace(value);
}

template <kiln::util::storable_c T>
auto SPSCQueue<T>::try_push(T&& value) -> bool
    requires std::move_constructible<T>
{
    return try_emplace(std::move(value));
}

template <kiln::util::storable_c T>
template <typename... Args_T>
    requires std::constructible_from<T, Args_T&&...>
auto SPSCQueue<T>::try_emplace(Args_T&&... args) -> bool
{
    const std::size_t tail{ m_tail.load(std::memory_order_relaxed) };

    if (tail - m_head_cache >= m_capacity)
    {
        m_head_cache = m_head.load(std::memory_order_acquire);
        if (tail - m_head_cache >= m_capacity)
        {
            return false;
        }
    }

    m_allocator.construct(std::addressof(at(tail)), std::forward<Args_T>(args)...);

    m_tail.store(tail + 1uz, std::memory_order_release);

    return true;
}

template <kiln::util::storable_c T>
template <std::ranges::input_range R>
    requires std::constructible_from<T, std::ranges::range_reference_t<R>>
auto SPSCQueue<T>::try_append_range(R&& values) -> std::size_t
{
    const std::size_t tail{ m_tail.load(std::memory_order_relaxed) };

    if (tail - m_head_cache >= m_capacity)
    {
        m_head_cache = m_head.load(std::memory_order_acquire);
    }

    const std::size_t free{ m_capacity - (tail - m_head_cache) };

    if (free == 0uz)
    {
        return 0uz;
    }

    if constexpr (!std::uses_allocator_v<T, std::pmr::polymorphic_allocator<T>>)
    {
        const std::size_t first_index{ tail & m_mask };
        const std::size_t first_length{ std::min(free, m_capacity - first_index) };

        const auto [second_input_iter, first_output_iter]
            = std::ranges::uninitialized_move(
                std::ranges::begin(values),
                std::ranges::end(values),
                std::addressof(at(first_index)),
                std::addressof(at(first_index)) + first_length
            );

        std::size_t count{
            std::distance(std::addressof(at(first_index)), first_output_iter)
        };

        const kiln::util::ScopeFail first_commit_guard{
            [this, tail, count] noexcept -> void
            {
                m_tail.store(tail + count, std::memory_order_release);   //
            },
        };

        if (const std::size_t second_length{ free - first_length }; second_length > 0uz)
        {
            const auto [_, second_output_iter] = std::ranges::uninitialized_move(
                second_input_iter,
                std::ranges::end(values),
                std::addressof(at(0uz)),
                std::addressof(at(second_length))
            );

            count += std::ranges::distance(std::addressof(at(0uz)), second_output_iter);
        }

        m_tail.store(tail + count, std::memory_order_release);

        return count;
    }
    else
    {
        std::size_t                 count{};
        const kiln::util::ScopeFail commit_guard{
            [this, tail, &count] noexcept -> void
            {
                m_tail.store(tail + count, std::memory_order_release);   //
            },
        };
        for (auto&& value : values)
        {
            if (count == free)
            {
                break;
            }
            m_allocator.construct(
                std::addressof(at(tail + count)),
                std::forward<decltype(value)>(value)
            );
            ++count;
        }

        m_tail.store(tail + count, std::memory_order_release);

        return count;
    }
}

template <kiln::util::storable_c T>
auto SPSCQueue<T>::pop() noexcept(std::is_nothrow_move_constructible_v<T>)
    -> std::optional<T>
    requires std::move_constructible<T>
{
    const std::size_t head{ m_head.load(std::memory_order_relaxed) };

    if (head == m_tail_cache)
    {
        m_tail_cache = m_tail.load(std::memory_order_acquire);
        if (head == m_tail_cache)
        {
            return std::nullopt;
        }
    }

    const kiln::util::ScopeFail destroy_guard{
        [this, head] noexcept -> void
        {
            m_allocator.destroy(std::addressof(at(head)));
            m_head.store(head + 1uz, std::memory_order_release);
        }
    };
    std::optional<T> result{ std::move(at(head)) };
    m_allocator.destroy(std::addressof(at(head)));

    m_head.store(head + 1uz, std::memory_order_release);

    return result;
}

template <kiln::util::storable_c T>
template <typename F>
    requires std::invocable<F, std::ranges::as_rvalue_view<std::span<T>>>
          && std::invocable<F, std::ranges::as_rvalue_view<split_span_view<T>>>
auto SPSCQueue<T>::pop_all(F&& callback) noexcept(
    std::is_nothrow_invocable_v<F, std::ranges::as_rvalue_view<std::span<T>>>
    && std::is_nothrow_invocable_v<F, std::ranges::as_rvalue_view<split_span_view<T>>>
) -> std::size_t
{
    const std::size_t head{ m_head.load(std::memory_order_relaxed) };

    if (head == m_tail_cache)
    {
        m_tail_cache = m_tail.load(std::memory_order_acquire);
    }

    const std::size_t count{ m_tail_cache - head };

    if (count == 0uz)
    {
        return 0uz;
    }

    const kiln::util::ScopeFail destroy_guard{
        [this, head, count] noexcept -> void
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                for (std::size_t i{}; i < count; ++i)
                {
                    m_allocator.destroy(std::addressof(at(head + i)));
                }
            }
            m_head.store(head + count, std::memory_order_release);
        }
    };

    if (const std::size_t first_length{ std::min(count, m_capacity - (head & m_mask)) };
        first_length == count)
    {
        std::invoke(
            std::forward<F>(callback),
            std::views::as_rvalue(std::span<T>{ std::addressof(at(head)), count })
        );
    }
    else
    {
        const std::size_t     second_length{ count - first_length };
        const split_span_view concatenated_spans{
            std::span{ std::addressof(at(head)),  first_length },
            std::span{  std::addressof(at(0uz)), second_length }
        };
        std::invoke(std::forward<F>(callback), std::views::as_rvalue(concatenated_spans));
    }

    if constexpr (!std::is_trivially_destructible_v<T>)
    {
        for (std::size_t i{}; i < count; ++i)
        {
            m_allocator.destroy(std::addressof(at(head + i)));
        }
    }

    m_head.store(head + count, std::memory_order_release);

    return count;
}

template <kiln::util::storable_c T>
auto SPSCQueue<T>::at(const std::size_t index) noexcept -> T&
{
    return m_data[index & m_mask];
}

}   // namespace demo
