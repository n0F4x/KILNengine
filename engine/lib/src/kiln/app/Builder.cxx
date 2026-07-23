module;

#include <memory_resource>

module kiln.app.Builder;

import kiln.app.App;
import kiln.reg.Registry;

namespace kiln::app {

Builder::Builder() : Builder{ Config{} } {}

auto Builder::build() && -> App
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        m_arena.make_transient_resource()
    };

    reg::Registry registry{
        std::move(m_registry_builder).build(transient_memory_resource)
    };

    return App{ std::move(m_arena), std::move(registry) };
}

}   // namespace kiln::app
