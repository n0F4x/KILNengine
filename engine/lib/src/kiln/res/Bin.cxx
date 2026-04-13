module;

#include <forward_list>
#include <memory_resource>

module kiln.res.Bin;

import kiln.res.GarbageBagInterface;
import kiln.util.containers.Polymorphic;

namespace kiln::res {

Bin::Bin(Bin&& other, const allocator_type& allocator)
    : m_memory_resource{ std::allocator_arg, allocator, allocator.resource() },
      m_garbage_bags{ std::move(other.m_garbage_bags), &*m_memory_resource }
{
}

Bin::Bin(const allocator_type& allocator)
    : m_memory_resource{ std::allocator_arg, allocator, allocator.resource() }
{
}

auto Bin::get_allocator() const noexcept -> allocator_type
{
    return m_memory_resource.get_allocator();
}

auto Bin::empty() const noexcept -> bool
{
    return m_garbage_bags.empty();
}

auto Bin::recycle() -> void
{
    m_garbage_bags.remove_if(
        [](GarbageBagWrapper& garbage_bag) -> bool
        {
            return garbage_bag->can_be_thrown_out();   //
        }
    );
}

auto Bin::Builder::build(app::MemoryArena& memory_arena) -> Bin
{
    return Bin{ memory_arena.pool_allocator() };
}

}   // namespace kiln::res
