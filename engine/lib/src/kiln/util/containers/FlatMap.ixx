module;

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

export module kiln.util.containers.FlatMap;

namespace kiln::util {

/*
 * Temporary until we can use std::flat_map
 */
export template <typename Key_T, typename Value_T>
class FlatMap final {
public:
    template <bool is_const_T>
    class Iterator;
    using ConstIterator = Iterator<true>;

    using reference       = std::pair<const Key_T&, Value_T&>;
    using const_reference = std::pair<const Key_T&, const Value_T&>;

    template <typename Self_T>
    [[nodiscard]]
    auto begin(this Self_T& self) noexcept -> Iterator<std::is_const_v<Self_T>>
    {
        return Iterator<std::is_const_v<Self_T>>{
            self.m_keys.begin(),
            self.m_values.begin(),
        };
    }

    [[nodiscard]]
    auto cbegin() const noexcept -> ConstIterator
    {
        return ConstIterator{
            m_keys.cbegin(),
            m_values.cbegin(),
        };
    }

    template <typename Self_T>
    [[nodiscard]]
    auto end(this Self_T& self) noexcept -> Iterator<std::is_const_v<Self_T>>
    {
        return Iterator<std::is_const_v<Self_T>>{
            self.m_keys.end(),
            self.m_values.end(),
        };
    }

    [[nodiscard]]
    auto cend() const noexcept -> ConstIterator
    {
        return ConstIterator{ m_keys.cend(), m_values.cend() };
    }

    [[nodiscard]]
    auto size() const noexcept -> std::size_t
    {
        return m_keys.size();
    }

    [[nodiscard]]
    auto contains(const Key_T& key) const noexcept -> bool
    {
        return std::ranges::contains(m_keys, key);
    }

    template <typename Self_T>
    [[nodiscard]]
    auto find(this Self_T& self, const Key_T& key) -> Iterator<std::is_const_v<Self_T>>
    {
        const auto key_iter   = std::ranges::find(self.m_keys, key);
        const auto value_iter = std::next(
            self.m_values.begin(),
            static_cast<std::ranges::range_difference_t<std::vector<Value_T>>>(
                std::ranges::distance(self.m_keys.begin(), key_iter)
            )
        );

        return Iterator<std::is_const_v<Self_T>>{ key_iter, value_iter };
    }

    template <typename UKey_T, typename... Args_T>
    auto try_emplace(UKey_T&& key, Args_T&&... args) -> std::pair<Iterator<false>, bool>
    {
        const auto found_iter = find(key);
        if (found_iter != cend())
        {
            return std::pair<Iterator<false>, bool>{ found_iter, false };
        }

        const auto key_iter = std::ranges::upper_bound(m_keys, key);
        const auto diff     = std::distance(m_keys.begin(), key_iter);
        const auto value_iter =
            std::next(m_values.begin(), std::distance(m_keys.begin(), key_iter));
        m_keys.emplace(key_iter, std::forward<UKey_T>(key));
        m_values.emplace(value_iter, std::forward<Args_T>(args)...);

        return std::pair<Iterator<false>, bool>{ std::next(begin(), diff), true };
    }

    auto erase(const ConstIterator iterator) -> std::size_t
    {
        if (iterator == cend())
        {
            return 0uz;
        }

        m_keys.erase(iterator.m_key_iter);
        m_values.erase(iterator.m_value_iter);

        return 1uz;
    }

    auto erase(const Key_T& key) -> std::size_t
    {
        return erase(find(key));
    }

private:
    std::vector<Key_T>   m_keys;
    std::vector<Value_T> m_values;
};

template <typename Key_T, typename Value_T>
template <bool is_const_T>
class FlatMap<Key_T, Value_T>::Iterator {
    friend FlatMap;
    template <bool>
    friend class Iterator;

    using KeyIterator   = std::vector<Key_T>::const_iterator;
    using ValueIterator = std::conditional_t<
        is_const_T,
        typename std::vector<Value_T>::const_iterator,
        typename std::vector<Value_T>::iterator>;
    using Reference = std::conditional_t<
        is_const_T,
        typename FlatMap::const_reference,
        typename FlatMap::reference>;

    struct ArrowProxy {
        Reference m_ref;

        auto operator->() -> Reference*
        {
            return std::addressof(m_ref);
        }
    };

public:
    using iterator_concept  = std::random_access_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = std::pair<Key_T, Value_T>;
    using difference_type   = std::ptrdiff_t;

    Iterator() = default;

    explicit(false) Iterator(Iterator<!is_const_T> other)
        requires is_const_T
        : Iterator{ other.m_key_iter, other.m_value_iter }
    {
    }

    Iterator(const KeyIterator key_iter, const ValueIterator value_iter)
        : m_key_iter(key_iter),
          m_value_iter(value_iter)
    {
    }

    auto operator==(const Iterator other) const noexcept -> bool
    {
        return m_key_iter == other.m_key_iter;
    }

    auto operator*() const -> Reference
    {
        return Reference{ *m_key_iter, *m_value_iter };
    }

    auto operator->() const -> ArrowProxy
    {
        return ArrowProxy{ **this };
    }

    auto operator++() -> Iterator&
    {
        ++m_key_iter;
        ++m_value_iter;
        return *this;
    }

    auto operator++(int) -> Iterator
    {
        Iterator result{ *this };
        ++m_key_iter;
        ++m_value_iter;
        return result;
    }

    auto operator+=(const difference_type diff) -> Iterator&
    {
        m_key_iter += diff;
        m_value_iter += diff;
        return *this;
    }

private:
    KeyIterator   m_key_iter;
    ValueIterator m_value_iter;
};

}   // namespace kiln::util
