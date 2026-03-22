module;

#include <algorithm>
#include <cstdint>
#include <deque>
#include <memory_resource>
#include <optional>
#include <string>
#include <string_view>

#include "kiln/util/contract_macros.hpp"

export module kiln.app.plugin.PluginStack;

import kiln.app.App;
import kiln.app.plugin.ErasedPlugin;
import kiln.app.plugin.hash_plugin;
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;
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


    auto build(App& app, std::pmr::memory_resource& transient_memory_resource) && -> void;

private:
    struct PluginNameChainNode {
        const PluginNameChainNode* previous{};
        std::string_view           plugin_name;

        [[nodiscard]]
        auto format() const -> std::string;

    private:
        auto format(std::string& out_string, uint32_t capacity) const -> void;
    };

    std::pmr::deque<uint64_t>     m_plugin_hashes;
    std::pmr::deque<ErasedPlugin> m_plugins;


    auto check_for_configuration_dependencies() const -> void;
    auto check_for_configuration_dependencies(const ErasedPlugin&) const -> void;

    auto check_for_cyclic_dependencies() const -> void;
    auto check_for_cyclic_dependencies(const ErasedPlugin&) const -> void;
    auto check_for_cyclic_dependency(
        uint64_t                   plugin_hash,
        std::string_view           plugin_name,
        std::string_view           configuration_plugin_name,
        uint64_t                   dependency_hash,
        const PluginNameChainNode& visited_plugin_names
    ) const -> void;

    auto fix_order(std::pmr::memory_resource& transient_memory_resource) -> void;
    auto fix_order(
        uint64_t                   plugin_hash,
        std::pmr::memory_resource& transient_memory_resource
    ) -> void;

    auto collect_all_resolved_dependency_plugin_hashes(
        const ErasedPlugin&        plugin,
        std::pmr::deque<uint64_t>& out
    ) const -> void;

    template <typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&, uint64_t plugin_hash) noexcept
        -> util::OptionalRef<util::const_like_t<ErasedPlugin, Self_T>>;
    template <typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&, uint64_t plugin_hash) noexcept
        -> util::const_like_t<ErasedPlugin, Self_T>&;
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
    return self.PluginStack::find(hash_plugin<Plugin_T>())
        .transform(
            [](util::const_like_t<ErasedPlugin, Self_T>& erased_plugin)
                -> util::const_like_t<Plugin_T, Self_T>&
            {
                return util::any_cast<Plugin_T>(erased_plugin);   //
            }
        );
}

template <typename Plugin_T, typename Self_T>
auto PluginStack::at(this Self_T& self) -> util::const_like_t<Plugin_T, Self_T>&
    requires(storable<Plugin_T>())
{
    return util::any_cast<Plugin_T>(self.PluginStack::at(hash_plugin<Plugin_T>()));
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

template <typename Self_T>
auto PluginStack::find(this Self_T& self, const uint64_t plugin_hash) noexcept
    -> util::OptionalRef<util::const_like_t<ErasedPlugin, Self_T>>
{
    const auto distance{
        static_cast<uint32_t>(
            std::distance(
                std::ranges::begin(self.PluginStack::m_plugin_hashes),
                std::ranges::find(
                    self.PluginStack::m_plugin_hashes,
                    plugin_hash
                )
            )   //
        )       //
    };
    if (distance == std::ranges::size(self.PluginStack::m_plugin_hashes))
    {
        return std::nullopt;
    }

    return self.PluginStack::m_plugins[distance];
}

template <typename Self_T>
auto PluginStack::at(this Self_T& self, uint64_t plugin_hash) noexcept
    -> util::const_like_t<ErasedPlugin, Self_T>&
{
    const util::OptionalRef found{ self.PluginStack::find(plugin_hash) };
    PRECOND(found.has_value());

    return *found;
}

}   // namespace kiln::app
