module;

#include <memory>
#include <memory_resource>
#include <utility>

export module kiln.app.App;

import kiln.app.context.Context;
import kiln.app.memory.Arena;
import kiln.util.Deleter;
import kiln.util.type_traits.forward_like;

namespace kiln::app {

export class App {
public:
    explicit App(Arena&& arena);

    template <typename Self_T>
    [[nodiscard]]
    auto context(this Self_T&&) noexcept -> util::forward_like_t<Context, Self_T>;

private:
    Arena m_arena;

    std::unique_ptr<std::pmr::memory_resource, util::Deleter> m_context_memory_resource;
    Context                                                   m_context;
};

}   // namespace kiln::app

namespace kiln::app {

inline App::App(Arena&& arena)
    : m_arena{ std::move(arena) },
      m_context_memory_resource{
          std::pmr::polymorphic_allocator{ &m_arena.pool_resource() }
              .new_object<std::pmr::monotonic_buffer_resource>(&m_arena.pool_resource()),
          util::Deleter{ std::pmr::polymorphic_allocator{ &m_arena.pool_resource() } }
      },
      m_context{ std::allocator_arg, m_context_memory_resource.get(), m_arena }
{
}

template <typename Self_T>
auto App::context(this Self_T&& self) noexcept -> util::forward_like_t<Context, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_context);
}

}   // namespace kiln::app
