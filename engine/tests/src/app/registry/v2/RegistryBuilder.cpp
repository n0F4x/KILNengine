#include <memory_resource>

#include <catch2/catch_test_macros.hpp>

import kiln.app.registry.BuildableEntry;
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
    struct Builder : EntryBuilderBase {
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
    struct Builder : EntryBuilderBase {
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

struct SimpleCyclicBuildEntryA;
struct SimpleCyclicBuildEntryB;

struct SimpleCyclicBuildEntryA
    : BuildableEntry<SimpleCyclicBuildEntryA, BuildDescriber<SimpleCyclicBuildEntryA>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SimpleCyclicBuildEntryB&) noexcept
            -> SimpleCyclicBuildEntryA
        {
            return SimpleCyclicBuildEntryA{};
        }
    };
};

struct SimpleCyclicBuildEntryB
    : BuildableEntry<SimpleCyclicBuildEntryB, BuildDescriber<SimpleCyclicBuildEntryB>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SimpleCyclicBuildEntryA&) noexcept
            -> SimpleCyclicBuildEntryB
        {
            return SimpleCyclicBuildEntryB{};
        }
    };
};

struct SimpleCyclicCreateEntryA;
struct SimpleCyclicCreateEntryB;

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

struct SimpleCyclicCreateEntryA::Builder : EntryBuilderBase {
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

struct SimpleCyclicCreateEntryB::Builder : EntryBuilderBase {
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
          BuildDescriber<SimpleEntryBuilderDependencyA>{}> {
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
          BuildDescriber<SimpleEntryBuilderDependencyB>{}> {
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

        SECTION("simple entry cycle")
        {
            SECTION("build")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<SimpleCyclicBuildEntryA>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                registry_builder.register_entry<SimpleCyclicCreateEntryA>();

                REQUIRE_THROWS_AS(
                    std::move(registry_builder).build(transient_memory_resource),
                    CyclicDependencyDetected
                );
            }
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
        // TODO: enable section
        // SECTION("automatically registered")
        // {
        //     RegistryBuilder registry_builder;
        //     registry_builder.register_entry<SimpleEntryBuilderDependencyB>(
        //         transient_memory_resource
        //     );
        //     Registry registry
        //         = std::move(registry_builder).build(transient_memory_resource);
        //
        //     REQUIRE(registry.contains<SimpleEntryBuilderDependencyA>());
        //     REQUIRE(registry.contains<SimpleEntryBuilderDependencyB>());
        // }
    }
}

}   // namespace kiln::app
