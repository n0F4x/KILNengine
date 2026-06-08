module;

#include <concepts>
#include <functional>
#include <memory>
#include <memory_resource>
#include <utility>

export module kiln.app.Builder;

import kiln.app.App;
import kiln.app.config.Config;
import kiln.app.config.ConfigBuilder;
import kiln.app.memory.MemoryArena;
import kiln.app.memory.MemoryArenaBuilder;
import kiln.app.registry.entry_builder_c;
import kiln.app.registry.entry_c;
import kiln.app.registry.Registry;
import kiln.app.registry.RegistryBuilder;
import kiln.util.containers.Indirect;
import kiln.util.type_traits.const_like;

namespace kiln::app {

export class Builder {
public:
    explicit Builder(const Config& config = Config{});


    template <entry_c Entry_T, typename Self_T>
    auto register_entry(this Self_T&&) -> Self_T&&;

    template <typename Self_T, typename Bundle_T>
        requires std::invocable<Bundle_T&&, Builder&>
    auto apply_bundle(this Self_T&&, Bundle_T&& bundle) -> Self_T&&;


    [[nodiscard]]
    auto build() && -> App;

private:
    MemoryArena     m_arena;
    RegistryBuilder m_registry_builder;
};

}   // namespace kiln::app

namespace kiln::app {

Builder::Builder(const Config& config) : m_registry_builder{ auto{ m_arena }, config } {}

template <entry_c Entry_T, typename Self_T>
auto Builder::register_entry(this Self_T&& self) -> Self_T&&
{
    self.Builder::m_registry_builder.template register_entry<Entry_T>();
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename Bundle_T>
    requires std::invocable<Bundle_T&&, Builder&>
auto Builder::apply_bundle(this Self_T&& self, Bundle_T&& bundle) -> Self_T&&
{
    std::invoke(std::forward<Bundle_T>(bundle), self);
    return std::forward<Self_T>(self);
}

auto Builder::build() && -> App
{
    auto transient_memory_resource{ m_arena.make_transient_resource() };

    Registry registry{ std::move(m_registry_builder).build(transient_memory_resource) };

    return App{
        std::move(m_arena),
        std::move(registry),
    };
}

}   // namespace kiln::app
