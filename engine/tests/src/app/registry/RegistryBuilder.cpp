#include <functional>
#include <memory_resource>

#include <catch2/catch_test_macros.hpp>

import kiln.app.registry.BuildableEntry;
import kiln.app.registry.BuildableEntryBuilder;
import kiln.app.registry.ConfigurationEntry;
import kiln.app.registry.CyclicDependencyDetected;
import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuildDirector;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.Registry;
import kiln.app.registry.RegistryBuilder;
import kiln.util.containers.OptionalRef;
import kiln.util.reflection;

namespace kiln::app {

namespace {

const std::string type_name{ util::name_of<RegistryBuilder>() };

template <typename Entry_T>
struct BuildDescriber {
    constexpr static auto operator()(EntryBuildDirector<Entry_T>& build_director) -> void
    {
        build_director.template use_builder<typename Entry_T::Builder>();
    }
};

struct SelfBuildDependentEntry
    : BuildableEntry<SelfBuildDependentEntry, BuildDescriber<SelfBuildDependentEntry>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SelfBuildDependentEntry&) noexcept
            -> SelfBuildDependentEntry
        {
            return SelfBuildDependentEntry{};
        }
    };
};

struct SelfCreateDependentEntry
    : BuildableEntry<SelfCreateDependentEntry, BuildDescriber<SelfCreateDependentEntry>{}>   //
{
    struct Builder : BuildableEntryBuilder {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto create(Builder&) noexcept -> Builder
        {
            return Builder{};
        }

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() noexcept -> SelfCreateDependentEntry
        {
            return SelfCreateDependentEntry{};
        }
    };
};

struct OptionalSelfBuildDependentEntry
    : BuildableEntry<
          OptionalSelfBuildDependentEntry,
          BuildDescriber<OptionalSelfBuildDependentEntry>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(
            util::OptionalRef<OptionalSelfBuildDependentEntry>
        ) noexcept -> OptionalSelfBuildDependentEntry
        {
            return OptionalSelfBuildDependentEntry{};
        }
    };
};

struct OptionalSelfCreateDependentEntry
    : BuildableEntry<
          OptionalSelfCreateDependentEntry,
          BuildDescriber<OptionalSelfCreateDependentEntry>{}>   //
{
    struct Builder : BuildableEntryBuilder {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto create(util::OptionalRef<Builder>) noexcept -> Builder
        {
            return Builder{};
        }

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() noexcept -> OptionalSelfCreateDependentEntry
        {
            return OptionalSelfCreateDependentEntry{};
        }
    };
};

struct SimpleEntryCyclicBuildEntryA;
struct SimpleEntryCyclicBuildEntryB;

struct SimpleEntryCyclicBuildEntryA
    : BuildableEntry<
          SimpleEntryCyclicBuildEntryA,
          BuildDescriber<SimpleEntryCyclicBuildEntryA>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SimpleEntryCyclicBuildEntryB&) noexcept
            -> SimpleEntryCyclicBuildEntryA
        {
            return SimpleEntryCyclicBuildEntryA{};
        }
    };
};

struct SimpleEntryCyclicBuildEntryB
    : BuildableEntry<
          SimpleEntryCyclicBuildEntryB,
          BuildDescriber<SimpleEntryCyclicBuildEntryB>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SimpleEntryCyclicBuildEntryA&) noexcept
            -> SimpleEntryCyclicBuildEntryB
        {
            return SimpleEntryCyclicBuildEntryB{};
        }
    };
};

struct SimpleBuilderCyclicBuildEntryA
    : BuildableEntry<
          SimpleBuilderCyclicBuildEntryA,
          BuildDescriber<SimpleBuilderCyclicBuildEntryA>{}>   //
{
    struct Builder;
};

struct SimpleBuilderCyclicBuildEntryB
    : BuildableEntry<
          SimpleBuilderCyclicBuildEntryB,
          BuildDescriber<SimpleBuilderCyclicBuildEntryB>{}>   //
{
    struct Builder;
};

struct SimpleBuilderCyclicBuildEntryA::Builder : BuildableEntryBuilder {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build(const SimpleBuilderCyclicBuildEntryB::Builder&) noexcept
        -> SimpleBuilderCyclicBuildEntryA
    {
        return SimpleBuilderCyclicBuildEntryA{};
    }
};

struct SimpleBuilderCyclicBuildEntryB::Builder : BuildableEntryBuilder {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build(const SimpleBuilderCyclicBuildEntryA::Builder&) noexcept
        -> SimpleBuilderCyclicBuildEntryB
    {
        return SimpleBuilderCyclicBuildEntryB{};
    }
};

struct SimpleCyclicCreateEntryA
    : BuildableEntry<SimpleCyclicCreateEntryA, BuildDescriber<SimpleCyclicCreateEntryA>{}>   //
{
    struct Builder;
};

struct SimpleCyclicCreateEntryB
    : BuildableEntry<SimpleCyclicCreateEntryB, BuildDescriber<SimpleCyclicCreateEntryB>{}>   //
{
    struct Builder;
};

struct SimpleCyclicCreateEntryA::Builder : BuildableEntryBuilder {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto create(SimpleCyclicCreateEntryB::Builder&) noexcept -> Builder
    {
        return Builder{};
    }

    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build() noexcept -> SimpleCyclicCreateEntryA
    {
        return SimpleCyclicCreateEntryA{};
    }
};

struct SimpleCyclicCreateEntryB::Builder : BuildableEntryBuilder {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto create(SimpleCyclicCreateEntryA::Builder&) noexcept -> Builder
    {
        return Builder{};
    }

    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build() noexcept -> SimpleCyclicCreateEntryB
    {
        return SimpleCyclicCreateEntryB{};
    }
};

struct SimpleEntryDependencyA : EntryBase {};

struct SimpleEntryDependencyB
    : BuildableEntry<SimpleEntryDependencyB, BuildDescriber<SimpleEntryDependencyB>{}> {
    constexpr explicit SimpleEntryDependencyB(SimpleEntryDependencyA&) noexcept {}

    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SimpleEntryDependencyA& a) noexcept
            -> SimpleEntryDependencyB
        {
            return SimpleEntryDependencyB{ a };
        }
    };
};

struct SimpleEntryBuilderDependencyA
    : BuildableEntry<
          SimpleEntryBuilderDependencyA,
          BuildDescriber<SimpleEntryBuilderDependencyA>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() noexcept -> SimpleEntryBuilderDependencyA
        {
            return SimpleEntryBuilderDependencyA{};
        }
    };
};

struct SimpleEntryBuilderDependencyB
    : BuildableEntry<
          SimpleEntryBuilderDependencyB,
          BuildDescriber<SimpleEntryBuilderDependencyB>{}>   //
{
    constexpr explicit SimpleEntryBuilderDependencyB(
        const SimpleEntryBuilderDependencyA::Builder&
    ) noexcept
    {
    }

    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(
            const SimpleEntryBuilderDependencyA::Builder& a_builder
        ) noexcept -> SimpleEntryBuilderDependencyB
        {
            return SimpleEntryBuilderDependencyB{ a_builder };
        }
    };
};

struct SimpleBuilderEntryDependencyA : ConfigurationEntry {};

struct SimpleBuilderEntryDependencyB
    : BuildableEntry<
          SimpleBuilderEntryDependencyB,
          BuildDescriber<SimpleBuilderEntryDependencyB>{}>   //
{
    constexpr explicit SimpleBuilderEntryDependencyB(
        SimpleBuilderEntryDependencyA&
    ) noexcept
    {
    }

    struct Builder : BuildableEntryBuilder {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto create(SimpleBuilderEntryDependencyA& a) noexcept -> Builder
        {
            return Builder{ a };
        }

        std::reference_wrapper<SimpleBuilderEntryDependencyA> a;

        // ReSharper disable once CppDFAUnreachableFunctionCall
        constexpr explicit Builder(SimpleBuilderEntryDependencyA& a) : a{ a } {}

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr auto build() const noexcept -> SimpleBuilderEntryDependencyB
        {
            return SimpleBuilderEntryDependencyB{ a };
        }
    };
};

}   // namespace

TEST_CASE(type_name)
{
    std::pmr::unsynchronized_pool_resource transient_memory_resource{
        std::pmr::get_default_resource()
    };

#ifdef KILN_DEBUG
    SECTION("cyclic dependency detection")
    {
        SECTION("self")
        {
            SECTION("build")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<SelfBuildDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<SelfCreateDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }
        }

        SECTION("optional self")
        {
            SECTION("build")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<OptionalSelfBuildDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<OptionalSelfCreateDependentEntry>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }
        }

        SECTION("simple entry -> entry cycle")
        {
            RegistryBuilder registry_builder;

            registry_builder.register_entry<SimpleEntryCyclicBuildEntryA>();

            REQUIRE_THROWS_AS(
                std::move(registry_builder).build(transient_memory_resource),
                CyclicDependencyDetected
            );
        }

        SECTION("simple entry -> builder cycle")
        {
            RegistryBuilder registry_builder;

            registry_builder.register_entry<SimpleBuilderCyclicBuildEntryA>();

            REQUIRE_THROWS_AS(
                std::move(registry_builder).build(transient_memory_resource),
                CyclicDependencyDetected
            );
        }

        SECTION("simple builder -> builder cycle")
        {
            RegistryBuilder registry_builder;

            registry_builder.register_entry<SimpleCyclicCreateEntryA>();

            REQUIRE_THROWS_AS(
                std::move(registry_builder).build(transient_memory_resource),
                CyclicDependencyDetected
            );
        }
    }
#endif

    SECTION("simple entry -> entry dependency")
    {
        SECTION("base")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleEntryDependencyA>();
            registry_builder.register_entry<SimpleEntryDependencyB>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleEntryDependencyA>());
            REQUIRE(registry.contains<SimpleEntryDependencyB>());
        }

        SECTION("reordered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleEntryDependencyB>();
            registry_builder.register_entry<SimpleEntryDependencyA>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleEntryDependencyA>());
            REQUIRE(registry.contains<SimpleEntryDependencyB>());
        }

        SECTION("automatically registered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleEntryDependencyB>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleEntryDependencyA>());
            REQUIRE(registry.contains<SimpleEntryDependencyB>());
        }
    }

    SECTION("simple entry -> builder dependency")
    {
        SECTION("base")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleEntryBuilderDependencyA>();
            registry_builder.register_entry<SimpleEntryBuilderDependencyB>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleEntryBuilderDependencyA>());
            REQUIRE(registry.contains<SimpleEntryBuilderDependencyB>());
        }

        SECTION("reordered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleEntryBuilderDependencyB>();
            registry_builder.register_entry<SimpleEntryBuilderDependencyA>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleEntryBuilderDependencyA>());
            REQUIRE(registry.contains<SimpleEntryBuilderDependencyB>());
        }

        SECTION("automatically registered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleEntryBuilderDependencyB>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleEntryBuilderDependencyA>());
            REQUIRE(registry.contains<SimpleEntryBuilderDependencyB>());
        }
    }

    SECTION("simple builder -> (configuration) entry dependency")
    {
        SECTION("base")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleBuilderEntryDependencyA>();
            registry_builder.register_entry<SimpleBuilderEntryDependencyB>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleBuilderEntryDependencyA>());
            REQUIRE(registry.contains<SimpleBuilderEntryDependencyB>());
        }

        SECTION("reordered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleBuilderEntryDependencyB>();
            registry_builder.register_entry<SimpleBuilderEntryDependencyA>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleBuilderEntryDependencyA>());
            REQUIRE(registry.contains<SimpleBuilderEntryDependencyB>());
        }

        SECTION("automatically registered")
        {
            RegistryBuilder registry_builder;
            registry_builder.register_entry<SimpleBuilderEntryDependencyB>();
            Registry registry
                = std::move(registry_builder).build(transient_memory_resource);

            REQUIRE(registry.contains<SimpleBuilderEntryDependencyA>());
            REQUIRE(registry.contains<SimpleBuilderEntryDependencyB>());
        }
    }
}

}   // namespace kiln::app
