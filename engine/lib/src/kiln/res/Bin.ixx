module;

#include <forward_list>
#include <memory_resource>
#include <utility>

export module kiln.res.Bin;

import kiln.app.memory.MemoryArena;
import kiln.res.GarbageBagInterface;
import kiln.util.containers.Indirect;
import kiln.util.containers.Polymorphic;

namespace kiln::res {

export class Bin {
public:
    class Builder;
    using allocator_type = std::pmr::polymorphic_allocator<>;


    template <typename T>
    [[nodiscard]]
    consteval static auto storable() -> bool;


    Bin(Bin&&, const allocator_type& allocator);

    explicit Bin() = default;
    explicit Bin(const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto empty() const noexcept -> bool;

    template <typename T>
    auto insert(T&& garbage_bag) -> T&
        requires(storable<T>() && std::is_constructible_v<T, T &&>);
    template <typename T, typename... Args_T>
    auto emplace(std::in_place_type_t<T>, Args_T&&... args) -> T&
        requires(storable<T>() && std::is_constructible_v<T, Args_T && ...>);

    auto recycle() -> void;

private:
    using GarbageBagWrapper = util::Polymorphic<GarbageBagInterface, true>;


    util::Indirect<std::pmr::unsynchronized_pool_resource> m_memory_resource{
        std::in_place
    };
    std::pmr::forward_list<GarbageBagWrapper> m_garbage_bags{ &*m_memory_resource };
};

class Bin::Builder {
public:
    [[nodiscard]]
    static auto build(app::MemoryArena& memory_arena) -> Bin;
};

}   // namespace kiln::res

namespace kiln::res {

template <typename T>
consteval auto Bin::storable() -> bool
{
    return GarbageBagWrapper::storable<T>();
}

template <typename T>
auto Bin::insert(T&& garbage_bag) -> T&
    requires(storable<T>() && std::is_constructible_v<T, T &&>)
{
    return emplace(
        std::in_place_type<std::remove_cvref_t<T>>, std::forward<T>(garbage_bag)
    );
}

template <typename T, typename... Args_T>
auto Bin::emplace(std::in_place_type_t<T> in_place_type, Args_T&&... args) -> T&
    requires(storable<T>() && std::is_constructible_v<T, Args_T && ...>)
{
    return static_cast<T&>(
        *m_garbage_bags.emplace_front(in_place_type, std::forward<Args_T>(args)...)
    );
}

}   // namespace kiln::res
