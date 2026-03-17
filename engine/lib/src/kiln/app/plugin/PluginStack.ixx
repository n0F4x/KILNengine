module;

#include <algorithm>
#include <deque>
#include <memory_resource>
#include <optional>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.plugin.PluginStack;

import kiln.app.App;
import kiln.app.plugin.ErasedPlugin;
import kiln.app.plugin.hash_plugin;
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.util.Any;
import kiln.util.contracts;
import kiln.util.OptionalRef;
import kiln.util.ScopeFail;
import kiln.util.type_traits.const_like;

namespace kiln::app {

export class PluginStack {
    template <typename T>
    [[nodiscard]]
    consteval static auto storable() -> bool;

public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;


    PluginStack() = default;
    explicit PluginStack(const allocator_type& allocator);
    PluginStack(const PluginStack&)     = delete;
    PluginStack(PluginStack&&) noexcept = default;
    PluginStack(PluginStack&&, const allocator_type& allocator);
    ~PluginStack();

    auto operator=(const PluginStack&) -> PluginStack&     = delete;
    auto operator=(PluginStack&&) noexcept -> PluginStack& = default;


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


    template <typename Plugin_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Plugin_T, Self_T>>
        requires(storable<Plugin_T>());

    template <typename Plugin_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&) -> util::const_like_t<Plugin_T, Self_T>&
        requires(storable<Plugin_T>());


    template <typename Plugin_T>
    auto insert(Plugin_T&& plugin) -> Plugin_T&
        requires(storable<std::remove_cvref_t<Plugin_T>>());


    auto build(App& app) && -> void;

private:
    std::pmr::deque<uint64_t>     m_plugin_hashes;
    std::pmr::deque<ErasedPlugin> m_plugins;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename T>
consteval auto PluginStack::storable() -> bool
{
    return ErasedPlugin::storable<T>();
}

template <typename Plugin_T, typename Self_T>
auto PluginStack::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<Plugin_T, Self_T>>
    requires(storable<Plugin_T>())
{
    const auto distance{
        static_cast<uint32_t>(
            std::distance(
                std::ranges::begin(self.PluginStack::m_plugin_hashes),
                std::ranges::find(
                    self.PluginStack::m_plugin_hashes,
                    hash_plugin<std::remove_const_t<Plugin_T>>()
                )
            )   //
        )       //
    };
    if (distance == std::ranges::size(self.PluginStack::m_plugin_hashes))
    {
        return std::nullopt;
    }

    return util::any_cast<Plugin_T>(self.PluginStack::m_plugins[distance]);
}

template <typename Plugin_T, typename Self_T>
auto PluginStack::at(this Self_T& self) -> util::const_like_t<Plugin_T, Self_T>&
    requires(storable<Plugin_T>())
{
    const util::OptionalRef found{ self.PluginStack::template find<Plugin_T>() };
    PRECOND(found.has_value());

    return *found;
}

template <typename Plugin_T>
auto PluginStack::insert(Plugin_T&& plugin) -> Plugin_T&
    requires(storable<std::remove_cvref_t<Plugin_T>>())
{
    PRECOND(!std::ranges::contains(
        m_plugin_hashes, hash_plugin<std::remove_cvref_t<Plugin_T>>()
    ));

    auto& result = util::any_cast<std::remove_cvref_t<Plugin_T>>(
        m_plugins.emplace_back(std::forward<Plugin_T>(plugin))
    );
    const util::ScopeFail plugin_guard{
        [this] noexcept -> void { m_plugins.pop_back(); },
    };

    m_plugin_hashes.push_back(hash_plugin<std::remove_cvref_t<Plugin_T>>());

    return result;
}

}   // namespace kiln::app
