module;

#include <concepts>
#include <memory_resource>
#include <utility>

export module kiln.app.Builder;

import kiln.app.App;
import kiln.app.config.Config;
import kiln.app.memory.MemoryArena;
import kiln.reg.configuration_entry_c;
import kiln.reg.entry_c;
import kiln.reg.Registry;
import kiln.reg.RegistryBuilder;

namespace kiln::app {

export class Builder {
public:
    explicit Builder();
    template <reg::decays_to_configuration_entry_c... Entries_T>
    explicit Builder(const Config& config, Entries_T&&... entries);


    template <reg::entry_c Entry_T, typename Self_T>
    auto register_entry(this Self_T&&) -> Self_T&&;

    template <typename Self_T, typename Bundle_T>
        requires std::invocable<Bundle_T&&, Builder&>
    auto apply_bundle(this Self_T&&, Bundle_T&& bundle) -> Self_T&&;


    [[nodiscard]]
    auto build() && -> App;

private:
    MemoryArena          m_arena;
    reg::RegistryBuilder m_registry_builder;
};

}   // namespace kiln::app

namespace kiln::app {

template <reg::decays_to_configuration_entry_c... Entries_T>
Builder::Builder(const Config& config, Entries_T&&... entries)
    : m_arena{},   // TODO (compiler bug): don't initialize m_arena explicitly
      m_registry_builder{
          std::allocator_arg,
          m_arena.pool_allocator(),
          auto{ m_arena },
          config,
          std::forward<Entries_T>(entries)...,
      }
{
}

template <reg::entry_c Entry_T, typename Self_T>
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

}   // namespace kiln::app
