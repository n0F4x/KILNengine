module;

#include <memory>
#include <type_traits>

export module kiln.exec.Ref;

import kiln.reg.entry_c;
import kiln.reg.Registry;
import kiln.exec.access_pattern_of;
import kiln.exec.AccessPattern;

namespace kiln::exec {

export template <typename T>
    requires reg::entry_c<std::remove_const_t<T>>
class Ref {
public:
    constexpr explicit Ref(T& ref) : m_ref{ std::addressof(ref) } {}

    [[nodiscard]]
    constexpr auto operator*() const noexcept -> T
    {
        return *m_ref;
    }

    [[nodiscard]]
    constexpr auto operator->() const noexcept -> T*
    {
        return m_ref;
    }

private:
    T* m_ref;
};

template <typename T>
constexpr AccessPattern access_pattern_of<Ref<T>>{
    std::is_const_v<T> ? AccessPattern::eRead : AccessPattern::eWrite,
};

export template <typename T>
[[nodiscard]]
auto provide_accessor(std::type_identity<Ref<T>>, reg::Registry& registry) -> Ref<T>
{
    return Ref<T>{ registry.at<std::remove_const_t<T>>() };
}

}   // namespace kiln::exec
