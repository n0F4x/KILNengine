module;

#include <memory>
#include <memory_resource>
#include <utility>

export module kiln.app.App;

import kiln.app.memory.Arena;
import kiln.app.resource.ResourceStack;
import kiln.util.Deleter;
import kiln.util.type_traits.forward_like;

namespace kiln::app {

export class App {
public:
    explicit App(Arena&& arena);

    template <typename Self_T>
    [[nodiscard]]
    auto arena(this Self_T&&) noexcept -> util::forward_like_t<Arena, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto resources(this Self_T&&) noexcept -> util::forward_like_t<ResourceStack, Self_T>;

private:
    Arena m_arena;
    std::unique_ptr<std::pmr::memory_resource, util::Deleter>
        m_resource_injection_stack_resource{
            std::pmr::polymorphic_allocator{ &m_arena.pool_resource() }
                .new_object<std::pmr::monotonic_buffer_resource>(&m_arena.pool_resource()),
            util::Deleter{ std::pmr::polymorphic_allocator{ &m_arena.pool_resource() } }
        };
    ResourceStack m_resources{ m_resource_injection_stack_resource.get() };
};

}   // namespace kiln::app

namespace kiln::app {

inline App::App(Arena&& arena) : m_arena{ std::move(arena) } {}

template <typename Self_T>
auto App::arena(this Self_T&& self) noexcept -> util::forward_like_t<Arena, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_arena);
}

template <typename Self_T>
auto App::resources(this Self_T&& self) noexcept
    -> util::forward_like_t<ResourceStack, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_resources);
}

}   // namespace kiln::app
