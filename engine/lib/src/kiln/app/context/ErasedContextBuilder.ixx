module;

#include <functional>
#include <string_view>

export module kiln.app.context.ErasedContextBuilder;

import kiln.app.context.ContextBuilderInterface;
import kiln.app.context.Contexts;
import kiln.util.concepts.naked;
import kiln.util.concepts.specialization_of;
import kiln.util.containers.Polymorphic;
import kiln.util.reflection;
import kiln.util.type_traits.result_of;

namespace kiln::app {

export class ErasedContextBuilder {
public:
    // NOLINTNEXTLINE(*-identifier-naming)
    using allocator_type = std::pmr::polymorphic_allocator<>;

    template <typename ContextBuilder_T>
    consteval static auto storable() -> bool;


    ErasedContextBuilder(ErasedContextBuilder&&, const allocator_type& allocator);

    template <typename ContextBuilder_T>
    explicit ErasedContextBuilder(
        std::allocator_arg_t,
        const allocator_type& allocator,
        ContextBuilder_T&&    context_builder
    )
        requires(
            !std::is_same_v<std::remove_cvref_t<ContextBuilder_T>, ErasedContextBuilder>
            && !util::specialization_of_c<
                std::remove_cvref_t<ContextBuilder_T>,
                std::in_place_type_t>
            && std::
                is_constructible_v<std::remove_cvref_t<ContextBuilder_T>, ContextBuilder_T &&>
            && storable<ContextBuilder_T>()
        );
    template <util::naked_c ContextBuilder_T, typename... Args_T>
    explicit ErasedContextBuilder(
        std::allocator_arg_t,
        const allocator_type& allocator,
        std::in_place_type_t<ContextBuilder_T>,
        Args_T&&... args
    )
        requires(
            std::is_constructible_v<ContextBuilder_T, Args_T && ...>
            && storable<ContextBuilder_T>()
        );


    [[nodiscard]]
    auto operator->() noexcept -> ContextBuilderInterface*;
    [[nodiscard]]
    auto operator->() const noexcept -> const ContextBuilderInterface*;

    [[nodiscard]]
    auto operator*() noexcept -> ContextBuilderInterface&;
    [[nodiscard]]
    auto operator*() const noexcept -> const ContextBuilderInterface&;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto name() const noexcept -> std::string_view;
    [[nodiscard]]
    auto hash() const noexcept -> uint64_t;

    auto configure_and_build(Contexts& contexts) && -> void;

private:
    struct VTable;
    template <typename ContextBuilder_T>
    struct Operations;

    util::Polymorphic<ContextBuilderInterface, true> m_context_builder;
    std::reference_wrapper<const VTable>             m_vtable;
};

}   // namespace kiln::app

namespace kiln::app {

struct ErasedContextBuilder::VTable {
    using NameFunc              = auto() -> std::string_view;
    using HashFunc              = auto() -> uint64_t;
    using ConfigureAndBuildFunc = auto(ErasedContextBuilder&&, Contexts& contexts)
        -> void;

    std::reference_wrapper<NameFunc>              name;
    std::reference_wrapper<HashFunc>              hash;
    std::reference_wrapper<ConfigureAndBuildFunc> configure_and_build;
};

template <typename ContextBuilder_T>
struct ErasedContextBuilder::Operations {
    constexpr static auto name() -> std::string_view
    {
        return util::name_of<ContextBuilder_T>();
    }

    constexpr static auto hash() -> uint64_t
    {
        return util::hash_u64<util::result_of_t<decltype(&ContextBuilder_T::build)>>();
    }

    static auto configure_and_build(
        ErasedContextBuilder&& erased_context_builder,
        Contexts&              contexts
    ) -> void
    {
        app::configure_and_build(
            static_cast<ContextBuilder_T&&>(*erased_context_builder.m_context_builder),
            contexts
        );
    }

    constexpr static VTable vtable{
        .name                = name,
        .hash                = hash,
        .configure_and_build = configure_and_build,
    };
};

template <typename ContextBuilder_T>
consteval auto ErasedContextBuilder::storable() -> bool
{
    return util::Polymorphic<ContextBuilderInterface, true>::storable<ContextBuilder_T>();
}

ErasedContextBuilder::ErasedContextBuilder(
    ErasedContextBuilder&& other,
    const allocator_type&  allocator
)
    : m_context_builder{ std::move(other.m_context_builder), allocator },
      m_vtable{ other.m_vtable }
{
}

template <typename ContextBuilder_T>
ErasedContextBuilder::ErasedContextBuilder(
    std::allocator_arg_t  allocator_arg,
    const allocator_type& allocator,
    ContextBuilder_T&&    context_builder
)
    requires(
        !std::is_same_v<std::remove_cvref_t<ContextBuilder_T>, ErasedContextBuilder>
        && !util::specialization_of_c<
            std::remove_cvref_t<ContextBuilder_T>,
            std::in_place_type_t>
        && std::
            is_constructible_v<std::remove_cvref_t<ContextBuilder_T>, ContextBuilder_T &&>
        && storable<ContextBuilder_T>()
    )
    : ErasedContextBuilder{
          allocator_arg,
          allocator,
          std::in_place_type<std::remove_cvref_t<ContextBuilder_T>>,
          std::forward<ContextBuilder_T>(context_builder),
      }
{
}

template <util::naked_c ContextBuilder_T, typename... Args_T>
ErasedContextBuilder::ErasedContextBuilder(
    std::allocator_arg_t                   allocator_arg,
    const allocator_type&                  allocator,
    std::in_place_type_t<ContextBuilder_T> in_place_type,
    Args_T&&... args
)
    requires(std::is_constructible_v<ContextBuilder_T, Args_T && ...>
             && storable<ContextBuilder_T>())
    : m_context_builder{
          allocator_arg,
          allocator,
          in_place_type,
          std::forward<Args_T>(args)...,
      },
      m_vtable{ Operations<ContextBuilder_T>::vtable }
{
}

auto ErasedContextBuilder::operator->() noexcept -> ContextBuilderInterface*
{
    return m_context_builder.operator->();
}

auto ErasedContextBuilder::operator->() const noexcept -> const ContextBuilderInterface*
{
    return m_context_builder.operator->();
}

auto ErasedContextBuilder::operator*() noexcept -> ContextBuilderInterface&
{
    return m_context_builder.operator*();
}

auto ErasedContextBuilder::operator*() const noexcept -> const ContextBuilderInterface&
{
    return m_context_builder.operator*();
}

auto ErasedContextBuilder::get_allocator() const noexcept -> allocator_type
{
    return m_context_builder.get_allocator();
}

auto ErasedContextBuilder::name() const noexcept -> std::string_view
{
    return m_vtable.get().name();
}

auto ErasedContextBuilder::hash() const noexcept -> uint64_t
{
    return m_vtable.get().hash();
}

auto ErasedContextBuilder::configure_and_build(Contexts& contexts) && -> void
{
    m_vtable.get().configure_and_build(std::move(*this), contexts);
}

}   // namespace kiln::app
