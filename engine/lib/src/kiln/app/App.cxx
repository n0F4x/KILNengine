module;

#include <utility>

module kiln.app.App;

namespace kiln::app {

App::App(MemoryArena&& memory_arena, reg::Registry&& registry)
    : m_arena{ memory_arena },
      m_registry{ std::move(registry) }
{
}

}   // namespace kiln::app
