module;

#include <utility>

export module kiln.app.App;

import kiln.app.memory.MemoryArena;
import kiln.reg.Registry;
import kiln.util.type_traits.forward_like;

namespace kiln::app {

export class App {
public:
    explicit App(MemoryArena&& memory_arena, reg::Registry&& registry);

    template <typename Self_T>
    [[nodiscard]]
    auto registry(this Self_T&&) noexcept -> util::forward_like_t<reg::Registry, Self_T>;

private:
    MemoryArena   m_arena;
    reg::Registry m_registry;
};

}   // namespace kiln::app

namespace kiln::app {

inline App::App(MemoryArena&& memory_arena, reg::Registry&& registry)
    : m_arena{ memory_arena },
      m_registry{ std::move(registry) }
{
}

template <typename Self_T>
auto App::registry(this Self_T&& self) noexcept
    -> util::forward_like_t<reg::Registry, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_registry);
}

}   // namespace kiln::app
