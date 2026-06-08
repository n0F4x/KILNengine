module;

#include <functional>
#include <string_view>

export module kiln.app.registry.ErasedEntryBuilder;

import kiln.app.registry.EntryBuilderInterface;
import kiln.app.registry.Registry;
import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.Polymorphic;
import kiln.util.reflection;
import kiln.util.type_traits.result_of;

namespace kiln::app {

export class ErasedEntryBuilder {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;

    template <typename EntryBuilder_T>
    consteval static auto storable() -> bool;


    ErasedEntryBuilder(ErasedEntryBuilder&&, const allocator_type& allocator);

    template <typename EntryBuilder_T>
    explicit ErasedEntryBuilder(
        std::allocator_arg_t,
        const allocator_type& allocator,
        EntryBuilder_T&&      entry_builder
    )
        requires(
            !std::is_same_v<std::remove_cvref_t<EntryBuilder_T>, ErasedEntryBuilder>
            && !util::specialization_of_c<
                std::remove_cvref_t<EntryBuilder_T>,
                std::in_place_type_t>
            && std::is_constructible_v<std::remove_cvref_t<EntryBuilder_T>, EntryBuilder_T &&>
            && storable<EntryBuilder_T>()
        );
    template <util::naked_c EntryBuilder_T, typename... Args_T>
    explicit ErasedEntryBuilder(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<EntryBuilder_T>,
        Args_T&&... args
    )
        requires(
            std::is_constructible_v<EntryBuilder_T, Args_T && ...>
            && storable<EntryBuilder_T>()
        );


    [[nodiscard]]
    auto operator->() noexcept -> EntryBuilderInterface*;
    [[nodiscard]]
    auto operator->() const noexcept -> const EntryBuilderInterface*;

    [[nodiscard]]
    auto operator*() noexcept -> EntryBuilderInterface&;
    [[nodiscard]]
    auto operator*() const noexcept -> const EntryBuilderInterface&;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto name() const noexcept -> std::string_view;
    [[nodiscard]]
    auto hash() const noexcept -> uint64_t;

    auto configure_and_build(Registry& registry) && -> void;

private:
    struct VTable;
    template <typename EntryBuilder_T>
    struct Operations;

    util::Polymorphic<EntryBuilderInterface, true> m_entry_builder;
    std::reference_wrapper<const VTable>           m_vtable;
};

}   // namespace kiln::app

namespace kiln::app {

struct ErasedEntryBuilder::VTable {
    using NameFunc              = auto() -> std::string_view;
    using HashFunc              = auto() -> uint64_t;
    using ConfigureAndBuildFunc = auto(ErasedEntryBuilder&&, Registry& registry) -> void;

    std::reference_wrapper<NameFunc>              name;
    std::reference_wrapper<HashFunc>              hash;
    std::reference_wrapper<ConfigureAndBuildFunc> configure_and_build;
};

template <typename EntryBuilder_T>
struct ErasedEntryBuilder::Operations {
    constexpr static auto name() -> std::string_view
    {
        return util::name_of<EntryBuilder_T>();
    }

    constexpr static auto hash() -> uint64_t
    {
        return util::hash_u64<util::result_of_t<decltype(&EntryBuilder_T::build)>>();
    }

    static auto configure_and_build(
        ErasedEntryBuilder&& erased_entry_builder,
        Registry&            registry
    ) -> void
    {
        app::configure_and_build(
            static_cast<EntryBuilder_T&&>(*erased_entry_builder.m_entry_builder),
            registry
        );
    }

    constexpr static VTable vtable{
        .name                = name,
        .hash                = hash,
        .configure_and_build = configure_and_build,
    };
};

template <typename EntryBuilder_T>
consteval auto ErasedEntryBuilder::storable() -> bool
{
    return util::Polymorphic<EntryBuilderInterface, true>::storable<EntryBuilder_T>();
}

ErasedEntryBuilder::ErasedEntryBuilder(
    ErasedEntryBuilder&&  other,
    const allocator_type& allocator
)
    : m_entry_builder{ std::move(other.m_entry_builder), allocator },
      m_vtable{ other.m_vtable }
{
}

template <typename EntryBuilder_T>
ErasedEntryBuilder::ErasedEntryBuilder(
    std::allocator_arg_t  allocator_arg,
    const allocator_type& allocator,
    EntryBuilder_T&&    entry_builder
)
    requires(
        !std::is_same_v<std::remove_cvref_t<EntryBuilder_T>, ErasedEntryBuilder>
        && !util::specialization_of_c<
            std::remove_cvref_t<EntryBuilder_T>,
            std::in_place_type_t>
        && std::is_constructible_v<std::remove_cvref_t<EntryBuilder_T>, EntryBuilder_T &&>
        && storable<EntryBuilder_T>()
    )
    : ErasedEntryBuilder{
          allocator_arg,
          allocator,
          std::in_place_type<std::remove_cvref_t<EntryBuilder_T>>,
          std::forward<EntryBuilder_T>(entry_builder),
      }
{
}

template <util::naked_c EntryBuilder_T, typename... Args_T>
ErasedEntryBuilder::ErasedEntryBuilder(
    std::allocator_arg_t                   allocator_arg,
    const allocator_type&                  allocator,
    std::in_place_type_t<EntryBuilder_T> in_place_type,
    Args_T&&... args
)
    requires(std::is_constructible_v<EntryBuilder_T, Args_T && ...>
             && storable<EntryBuilder_T>())
    : m_entry_builder{
          allocator_arg,
          allocator,
          in_place_type,
          std::forward<Args_T>(args)...,
      },
      m_vtable{ Operations<EntryBuilder_T>::vtable }
{
}

auto ErasedEntryBuilder::operator->() noexcept -> EntryBuilderInterface*
{
    return m_entry_builder.operator->();
}

auto ErasedEntryBuilder::operator->() const noexcept -> const EntryBuilderInterface*
{
    return m_entry_builder.operator->();
}

auto ErasedEntryBuilder::operator*() noexcept -> EntryBuilderInterface&
{
    return m_entry_builder.operator*();
}

auto ErasedEntryBuilder::operator*() const noexcept -> const EntryBuilderInterface&
{
    return m_entry_builder.operator*();
}

auto ErasedEntryBuilder::get_allocator() const noexcept -> allocator_type
{
    return m_entry_builder.get_allocator();
}

auto ErasedEntryBuilder::name() const noexcept -> std::string_view
{
    return m_vtable.get().name();
}

auto ErasedEntryBuilder::hash() const noexcept -> uint64_t
{
    return m_vtable.get().hash();
}

auto ErasedEntryBuilder::configure_and_build(Registry& registry) && -> void
{
    m_vtable.get().configure_and_build(std::move(*this), registry);
}

}   // namespace kiln::app
