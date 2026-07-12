module;

#include <cstdint>
#include <memory_resource>
#include <span>
#include <type_traits>
#include <vector>

export module kiln.exec.Task;

import kiln.exec.Access;
import kiln.exec.accessor_c;
import kiln.exec.AccessPattern;
import kiln.reg.Registry;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.reflection;

namespace kiln::exec {

export template <typename Result_T>
class Task {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    Task(Task&&, const allocator_type&);

    template <decays_to_accessor_c... Accessors_T>
    explicit Task(auto (&func)(Accessors_T...)->Result_T, reg::Registry& registry);
    template <decays_to_accessor_c... Accessors_T>
    explicit Task(
        std::allocator_arg_t,
        const allocator_type& allocator,
        auto (&func)(Accessors_T...)->Result_T,
        reg::Registry& registry
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto accessed_types() const noexcept -> std::span<const uint64_t>;
    [[nodiscard]]
    auto access_patterns() const noexcept -> std::span<const AccessPattern>;


    auto operator()() -> Result_T;

private:
    std::pmr::vector<uint64_t>         m_accessed_types;
    std::pmr::vector<AccessPattern>    m_access_patterns;
    util::MoveOnlyFunction<Result_T()> m_invoke;
};

}   // namespace kiln::exec

namespace kiln::exec {

template <typename Result_T>
Task<Result_T>::Task(Task&& other, const allocator_type& allocator)
    : m_accessed_types{ std::move(other.m_accessed_types), allocator },
      m_access_patterns{ std::move(other.m_access_patterns), allocator },
      m_invoke{ std::move(other.m_invoke), allocator }
{
}

template <typename Result_T>
template <decays_to_accessor_c... Accessors_T>
Task<Result_T>::Task(auto (&func)(Accessors_T...)->Result_T, reg::Registry& registry)
    : Task{ std::allocator_arg, std::pmr::get_default_resource(), func, registry }
{
}

template <typename Result_T>
template <decays_to_accessor_c... Accessors_T>
Task<Result_T>::Task(
    std::allocator_arg_t,
    const allocator_type& allocator,
    auto (&func)(Accessors_T...)->Result_T,
    reg::Registry& registry
)
    : m_accessed_types{ allocator },
      m_access_patterns{ allocator },
      m_invoke{
          [func,
           ... accessors = provide_accessor(
               std::type_identity<std::remove_cvref_t<Accessors_T>>{},
               registry
           )] mutable -> Result_T
          {
              return func(accessors...);   //
          },
      }
{
    const std::array<std::span<const Access>, sizeof...(Accessors_T)> accesses{
        accesses_of(std::type_identity<std::remove_cvref_t<Accessors_T>>{})...
    };

    std::size_t access_count{};
    for (const std::span<const Access> individual_accesses : accesses)
    {
        access_count += individual_accesses.size();
    }

    m_accessed_types.reserve(access_count);
    m_access_patterns.reserve(access_count);

    for (const std::span<const Access> individual_accesses : accesses)
    {
        // ReSharper disable once CppUseStructuredBinding
        for (const Access& access : individual_accesses)
        {
            m_accessed_types.push_back(access.type_hash);
            m_access_patterns.push_back(access.access_pattern);
        }
    }
}

template <typename Result_T>
auto Task<Result_T>::get_allocator() const noexcept -> allocator_type
{
    return m_accessed_types.get_allocator();
}

template <typename Result_T>
auto Task<Result_T>::accessed_types() const noexcept -> std::span<const uint64_t>
{
    return m_accessed_types;
}

template <typename Result_T>
auto Task<Result_T>::access_patterns() const noexcept -> std::span<const AccessPattern>
{
    return m_access_patterns;
}

template <typename Result_T>
auto Task<Result_T>::operator()() -> Result_T
{
    return m_invoke();
}

}   // namespace kiln::exec
