module;

#include <functional>
#include <string_view>

export module kiln.app.plugin.ErasedPlugin;

import kiln.app.App;
import kiln.app.plugin.hash_plugin;
import kiln.app.plugin.PluginInterface;
import kiln.util.containers.Polymorphic;
import kiln.util.reflection;

namespace kiln::app {

export class ErasedPlugin {
public:
    // NOLINTNEXTLINE(*-identifier-naming)
    using allocator_type = std::pmr::polymorphic_allocator<>;

    template <typename Plugin_T>
    consteval static auto storable() -> bool;


    ErasedPlugin(ErasedPlugin&&) noexcept;
    ErasedPlugin(ErasedPlugin&&, const allocator_type& allocator);

    template <typename Plugin_T>
    ErasedPlugin(std::allocator_arg_t, const allocator_type& allocator, Plugin_T&& plugin)
        requires(!std::same_as<Plugin_T, ErasedPlugin> && storable<Plugin_T>());

    auto operator=(ErasedPlugin&&) noexcept -> ErasedPlugin& = default;

    [[nodiscard]]
    auto operator->() noexcept -> PluginInterface*;
    [[nodiscard]]
    auto operator->() const noexcept -> const PluginInterface*;

    [[nodiscard]]
    auto operator*() noexcept -> PluginInterface&;
    [[nodiscard]]
    auto operator*() const noexcept -> const PluginInterface&;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto name() const noexcept -> std::string_view;
    [[nodiscard]]
    auto hash() const noexcept -> uint64_t;

    auto build(App& app) && -> void;

private:
    struct VTable;
    template <typename Plugin_T>
    struct Operations;

    util::Polymorphic<PluginInterface, true> m_plugin;
    std::reference_wrapper<const VTable>     m_vtable;
};

}   // namespace kiln::app

namespace kiln::app {

struct ErasedPlugin::VTable {
    using NameFunc  = auto() -> std::string_view;
    using HashFunc  = auto() -> uint64_t;
    using BuildFunc = auto(ErasedPlugin&&, App& app) -> void;

    std::reference_wrapper<NameFunc>  name;
    std::reference_wrapper<HashFunc>  hash;
    std::reference_wrapper<BuildFunc> build;
};

template <typename Plugin_T>
struct ErasedPlugin::Operations {
    constexpr static auto name() -> std::string_view
    {
        return util::name_of<Plugin_T>();
    }

    constexpr static auto hash() -> uint64_t
    {
        return hash_plugin<Plugin_T>();
    }

    static auto build(ErasedPlugin&& erased_plugin, App& app) -> void
    {
        app::build(static_cast<Plugin_T&&>(*erased_plugin.m_plugin), app);
    }

    constexpr static VTable vtable{
        .name  = name,
        .hash  = hash,
        .build = build,
    };
};

template <typename Plugin_T>
consteval auto ErasedPlugin::storable() -> bool
{
    return util::Polymorphic<PluginInterface, true>::storable<Plugin_T>();
}

ErasedPlugin::ErasedPlugin(ErasedPlugin&& other) noexcept
    : m_plugin{ std::move(other.m_plugin) },
      m_vtable{ other.m_vtable }
{
}

ErasedPlugin::ErasedPlugin(ErasedPlugin&& other, const allocator_type& allocator)
    : m_plugin{ std::move(other.m_plugin), allocator },
      m_vtable{ other.m_vtable }
{
}

template <typename Plugin_T>
ErasedPlugin::ErasedPlugin(
    std::allocator_arg_t  allocator_arg,
    const allocator_type& allocator,
    Plugin_T&&            plugin
)
    requires(!std::same_as<Plugin_T, ErasedPlugin> && storable<Plugin_T>())
    : m_plugin{ allocator_arg, allocator, std::forward<Plugin_T>(plugin) },
      m_vtable{ Operations<std::remove_cvref_t<Plugin_T>>::vtable }
{
}

auto ErasedPlugin::operator->() noexcept -> PluginInterface*
{
    return m_plugin.operator->();
}

auto ErasedPlugin::operator->() const noexcept -> const PluginInterface*
{
    return m_plugin.operator->();
}

auto ErasedPlugin::operator*() noexcept -> PluginInterface&
{
    return m_plugin.operator*();
}

auto ErasedPlugin::operator*() const noexcept -> const PluginInterface&
{
    return m_plugin.operator*();
}

auto ErasedPlugin::get_allocator() const noexcept -> allocator_type
{
    return m_plugin.get_allocator();
}

auto ErasedPlugin::name() const noexcept -> std::string_view
{
    return m_vtable.get().name();
}

auto ErasedPlugin::hash() const noexcept -> uint64_t
{
    return m_vtable.get().hash();
}

auto ErasedPlugin::build(App& app) && -> void
{
    m_vtable.get().build(std::move(*this), app);
}

}   // namespace kiln::app
