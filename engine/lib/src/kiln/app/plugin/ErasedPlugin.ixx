module;

#include <functional>
#include <span>
#include <string_view>

export module kiln.app.plugin.ErasedPlugin;

import kiln.app.App;
import kiln.app.plugin.hash_plugin;
import kiln.app.plugin.meta_plugin_c;
import kiln.app.plugin.plugin_c;
import kiln.app.plugin.PluginInterface;
import kiln.util.containers.Any;
import kiln.util.reflection;

namespace kiln::app {

template <typename T>
struct ErasedPluginPolicy {
    constexpr static bool value{ plugin_c<T> || meta_plugin_c<T> };
};

template <typename ErasedPlugin_T>
class ErasedPluginInterfaceMixin {
public:
    explicit ErasedPluginInterfaceMixin(
        const util::AnyExtraVTableAccessor extra_vtable_accessor
    )
        : m_extra_vtable{ extra_vtable_accessor }
    {
    }

    [[nodiscard]]
    auto name() const -> std::string_view
    {
        return m_extra_vtable(static_cast<const ErasedPlugin_T&>(*this)).name();
    }

    [[nodiscard]]
    auto hash() const -> uint64_t
    {
        return m_extra_vtable(static_cast<const ErasedPlugin_T&>(*this)).hash();
    }

    [[nodiscard]]
    auto configuration_dependency_hash_set() const -> std::span<const uint64_t>
    {
        return m_extra_vtable(static_cast<const ErasedPlugin_T&>(*this))
            .configuration_dependency_hash_set(static_cast<const ErasedPlugin_T&>(*this));
    }

    [[nodiscard]]
    auto dependency_hash_set() const -> std::span<const uint64_t>
    {
        return m_extra_vtable(static_cast<const ErasedPlugin_T&>(*this))
            .dependency_hash_set(static_cast<const ErasedPlugin_T&>(*this));
    }

    auto build(App& app) && -> void
    {
        m_extra_vtable(static_cast<ErasedPlugin_T&>(*this))
            .build(std::move(static_cast<ErasedPlugin_T&>(*this)), app);
    }

private:
    util::AnyExtraVTableAccessor m_extra_vtable;
};

template <typename ErasedPlugin_T>
struct ErasedPluginExtraVTable {
    using NameFunc                           = auto() -> std::string_view;
    using HashFunc                           = auto() -> uint64_t;
    using ConfigurationDependencyHashSetFunc = auto(const ErasedPlugin_T&)
        -> std::span<const uint64_t>;
    using DependencyHashSetFunc = auto(const ErasedPlugin_T&)
        -> std::span<const uint64_t>;
    using BuildFunc = auto(ErasedPlugin_T&&, App& app) -> void;

    std::reference_wrapper<NameFunc> name;
    std::reference_wrapper<HashFunc> hash;
    std::reference_wrapper<ConfigurationDependencyHashSetFunc>
                                                  configuration_dependency_hash_set;
    std::reference_wrapper<DependencyHashSetFunc> dependency_hash_set;
    std::reference_wrapper<BuildFunc>             build;

    template <typename Plugin_T>
    struct Operations {
        constexpr static auto name() -> std::string_view
        {
            return util::name_of<Plugin_T>();
        }

        constexpr static auto hash() -> uint64_t
        {
            return hash_plugin<Plugin_T>();
        }

        static auto configuration_dependency_hash_set(const ErasedPlugin_T& erased_plugin)
            -> std::span<const uint64_t>
        {
            return app::configuration_dependency_hash_set(
                util::any_cast<Plugin_T>(erased_plugin)
            );
        }

        static auto dependency_hash_set(const ErasedPlugin_T& erased_plugin)
            -> std::span<const uint64_t>
        {
            return app::dependency_hash_set(util::any_cast<Plugin_T>(erased_plugin));
        }

        static auto build(ErasedPlugin_T&& erased_plugin, App& app) -> void
        {
            app::build(util::any_cast<Plugin_T>(std::move(erased_plugin)), app);
        }

        constexpr static ErasedPluginExtraVTable vtable{
            .name                              = name,
            .hash                              = hash,
            .configuration_dependency_hash_set = configuration_dependency_hash_set,
            .dependency_hash_set               = dependency_hash_set,
            .build                             = build,
        };
    };
};

export using ErasedPlugin = util::BasicAny<util::DefaultAnyTraits<
    true,
    0,
    0,
    ErasedPluginPolicy,
    ErasedPluginInterfaceMixin,
    ErasedPluginExtraVTable>>;

}   // namespace kiln::app
