module;

#include <cstdint>
#include <flat_map>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <vector>

export module kiln.exec.Task;

import kiln.exec.access_pattern_of;
import kiln.exec.accessor_c;
import kiln.exec.AccessPattern;
import kiln.reg.Registry;
import kiln.util.containers.MoveOnlyFunction;
import kiln.util.reflection;

namespace kiln::exec {

export class Task {
    using Accesses = std::flat_map<
        uint64_t,
        AccessPattern,
        std::less<>,
        std::pmr::vector<uint64_t>,
        std::pmr::vector<AccessPattern>>;

public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    Task(Task&&, const allocator_type&);

    template <decays_to_accessor_c... Accessors_T>
    explicit Task(auto (&func)(Accessors_T...)->void, reg::Registry& registry);
    template <decays_to_accessor_c... Accessors_T>
    explicit Task(
        std::allocator_arg_t,
        const allocator_type& allocator,
        auto (&func)(Accessors_T...)->void,
        reg::Registry& registry
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto accesses() const noexcept -> const Accesses&;


    auto operator()() -> void;

private:
    Accesses                       m_accesses;
    util::MoveOnlyFunction<void()> m_invoke;
};

}   // namespace kiln::exec

namespace kiln::exec {

template <decays_to_accessor_c... Accessors_T>
Task::Task(auto (&func)(Accessors_T...)->void, reg::Registry& registry)
    : Task{ std::allocator_arg, std::pmr::get_default_resource(), func, registry }
{
}

template <decays_to_accessor_c... Accessors_T>
Task::Task(
    std::allocator_arg_t,
    const allocator_type& allocator,
    auto (&func)(Accessors_T...)->void,
    reg::Registry& registry
)
    : m_accesses{
          {
           std::pair{
                  util::hash_u64<std::remove_cvref_t<Accessors_T>>(),
                  access_pattern_of<std::remove_cvref_t<Accessors_T>>,
              }...,
           },
          allocator
},
      m_invoke{
          [func,
           ... accessors = provide_accessor(
               std::type_identity<std::remove_cvref_t<Accessors_T>>{},
               registry
           )] mutable -> void
          {
              func(accessors...);   //
          },
      }
{
}

}   // namespace kiln::exec
