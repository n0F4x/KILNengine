module;

#include <memory_resource>
#include <utility>

export module kiln.app.App;

import kiln.app.context.Contexts;
import kiln.app.memory.MemoryArena;
import kiln.util.type_traits.forward_like;

namespace kiln::app {

export class App {
public:
    explicit App(MemoryArena&& memory_arena, Contexts&& contexts);

    template <typename Self_T>
    [[nodiscard]]
    auto contexts(this Self_T&&) noexcept -> util::forward_like_t<Contexts, Self_T>;

private:
    MemoryArena m_arena;
    Contexts    m_contexts;
};

}   // namespace kiln::app

namespace kiln::app {

inline App::App(MemoryArena&& memory_arena, Contexts&& contexts)
    : m_arena{ std::move(memory_arena) },
      m_contexts{ std::move(contexts) }
{
}

template <typename Self_T>
auto App::contexts(this Self_T&& self) noexcept -> util::forward_like_t<Contexts, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_contexts);
}

}   // namespace kiln::app
