#include <memory_resource>

#include <catch2/catch_test_macros.hpp>

import kiln.app.registry.BuildableEntry;
import kiln.app.registry.CyclicDependencyDetected;
import kiln.app.registry.EntryBuildDirector;
import kiln.app.registry.EntryBuilderBase;
import kiln.app.registry.RegistryBuilder;
import kiln.util.containers.OptionalRef;
import kiln.util.reflection;

namespace kiln::app {

namespace {

const std::string type_name{ util::name_of<RegistryBuilder>() };

template <typename Entry_T>
struct BuildDescriber {
    constexpr static auto operator()(EntryBuildDirector<Entry_T>& build_director) -> void;
};

struct SelfBuildDependentEntry
    : BuildableEntry<SelfBuildDependentEntry, BuildDescriber<SelfBuildDependentEntry>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(SelfBuildDependentEntry&) -> SelfBuildDependentEntry
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
        constexpr static auto create(Builder&) -> Builder
        {
            return Builder{};
        }

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() -> SelfCreateDependentEntry
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
        constexpr static auto build(util::OptionalRef<OptionalSelfBuildDependentEntry>)
            -> OptionalSelfBuildDependentEntry
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
        constexpr static auto create(util::OptionalRef<Builder>) -> Builder
        {
            return Builder{};
        }

        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build() -> OptionalSelfCreateDependentEntry
        {
            return OptionalSelfCreateDependentEntry{};
        }
    };
};

template <typename Entry_T>
constexpr auto BuildDescriber<Entry_T>::operator()(
    EntryBuildDirector<Entry_T>& build_director
) -> void
{
    build_director.template use_builder<typename Entry_T::Builder>();
}

struct SimpleCyclicBuildEntryA;
struct SimpleCyclicBuildEntryB;

struct SimpleCyclicBuildEntryA
    : BuildableEntry<SimpleCyclicBuildEntryA, BuildDescriber<SimpleCyclicBuildEntryA>{}>   //
{
    struct Builder : EntryBuilderBase {
        [[nodiscard]]
        // ReSharper disable once CppDeclaratorNeverUsed
        constexpr static auto build(util::OptionalRef<SimpleCyclicBuildEntryB>)
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
        constexpr static auto build(util::OptionalRef<SimpleCyclicBuildEntryA>)
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
    constexpr static auto create(util::OptionalRef<SimpleCyclicCreateEntryB::Builder>)
        -> Builder
    {
        return Builder{};
    }

    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build() -> SimpleCyclicCreateEntryA
    {
        return SimpleCyclicCreateEntryA{};
    }
};

struct SimpleCyclicCreateEntryB::Builder : EntryBuilderBase {
    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto create(util::OptionalRef<SimpleCyclicCreateEntryA::Builder>)
        -> Builder
    {
        return Builder{};
    }

    [[nodiscard]]
    // ReSharper disable once CppDeclaratorNeverUsed
    constexpr static auto build() -> SimpleCyclicCreateEntryB
    {
        return SimpleCyclicCreateEntryB{};
    }
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

                REQUIRE_THROWS_AS(
                    registry_builder.register_entry<SelfBuildDependentEntry>(
                        transient_memory_resource
                    ),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                REQUIRE_THROWS_AS(
                    registry_builder.register_entry<SelfCreateDependentEntry>(
                        transient_memory_resource
                    ),
                    CyclicDependencyDetected
                );
            }
        }

        SECTION("optional self")
        {
            SECTION("build")
            {
                RegistryBuilder registry_builder;

                REQUIRE_THROWS_AS(
                    registry_builder.register_entry<OptionalSelfBuildDependentEntry>(
                        transient_memory_resource
                    ),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                REQUIRE_THROWS_AS(
                    registry_builder.register_entry<OptionalSelfCreateDependentEntry>(
                        transient_memory_resource
                    ),
                    CyclicDependencyDetected
                );
            }
        }

        SECTION("simple entry cycle")
        {
            SECTION("build")
            {
                RegistryBuilder registry_builder;

                REQUIRE_THROWS_AS(
                    registry_builder.register_entry<SimpleCyclicBuildEntryA>(
                        transient_memory_resource
                    ),
                    CyclicDependencyDetected
                );
            }

            SECTION("create")
            {
                RegistryBuilder registry_builder;

                REQUIRE_THROWS_AS(
                    registry_builder.register_entry<SimpleCyclicCreateEntryA>(
                        transient_memory_resource
                    ),
                    CyclicDependencyDetected
                );
            }
        }
    }
#endif
}

}   // namespace kiln::app
