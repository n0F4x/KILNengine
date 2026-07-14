module;

#include <memory>
#include <span>
#include <type_traits>

export module kiln.exec.Ref;

import kiln.reg.entry_c;
import kiln.reg.Registry;
import kiln.exec.Access;
import kiln.exec.AccessPattern;
import kiln.util.reflection;

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

export template <typename T>
[[nodiscard]]
constexpr auto accesses_of(std::type_identity<Ref<T>>) noexcept -> std::span<const Access>
{
    constexpr static std::array accesses{
        Access{
               .access_pattern = std::is_const_v<T>   //
                                ? AccessPattern::eRead
                                : AccessPattern::eWrite,
               .resource_id    = util::hash_u64<std::remove_const_t<T>>(),
               },
    };

    return std::span{ accesses };
}

export template <typename T>
[[nodiscard]]
auto provide_accessor(std::type_identity<Ref<T>>, reg::Registry& registry) -> Ref<T>
{
    return Ref<T>{ registry.at<std::remove_const_t<T>>() };
}

}   // namespace kiln::exec
