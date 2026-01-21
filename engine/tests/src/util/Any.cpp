#include <array>
#include <type_traits>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "kiln/util/Any.hpp"

TEST_CASE("util::Any")
{
    class Value {
    public:
        Value()             = default;
        Value(const Value&) = default;

        constexpr Value(Value&& other) noexcept
            : m_value{ std::exchange(other.m_value, 0) }
        {
        }

        constexpr explicit Value(const int value) noexcept : m_value{ value } {}

        auto operator=(const Value&) -> Value& = default;

        constexpr auto operator=(Value&& other) noexcept -> Value&
        {
            m_value = std::exchange(other.m_value, 0);
            return *this;
        }

        auto operator<=>(const Value&) const = default;

        [[nodiscard]]
        constexpr auto value() const noexcept -> int
        {
            return m_value;
        }

    private:
        int m_value{};
    };

    constexpr static Value value{ 2 };
    constexpr static Value other_value{ 3 };

    SECTION("in_place construct")
    {
        kiln::util::Any any(std::in_place_type<Value>, value.value());
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("forwarding construct")
    {
        const kiln::util::Any any{ value };
        REQUIRE(any_cast<Value>(any) == value);
    }

    SECTION("copy construct")
    {
        const kiln::util::Any any{ value };

        const kiln::util::Any copy{ any };
        REQUIRE(any_cast<Value>(any) == any_cast<Value>(copy));
    }

    SECTION("move construct")
    {
        kiln::util::Any any{ value };

        const kiln::util::Any moved_to{ std::move(any) };
        REQUIRE(any_cast<Value>(moved_to) == value);
    }

    SECTION("copy assignment")
    {
        const kiln::util::Any any{ value };
        kiln::util::Any       copy{ other_value };

        copy = any;
        REQUIRE(any_cast<Value>(copy) == any_cast<Value>(any));
    }

    SECTION("move assignment")
    {
        kiln::util::Any moved_from{ value };
        kiln::util::Any moved_to{ other_value };

        moved_to = std::move(moved_from);
        REQUIRE(any_cast<Value>(moved_to) == value);

        moved_from = std::move(moved_to);
        REQUIRE(any_cast<Value>(moved_from) == value);
    }

    SECTION("get &")
    {
        kiln::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), Value&>);
        assert(result == value);
    }

    SECTION("get const&")
    {
        const kiln::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(any);

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&>);
        REQUIRE(result == value);
    }

    SECTION("get &&")
    {
        kiln::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        const auto result = any_cast<Value>(std::move(any));
        REQUIRE(result == value);

        decltype(auto) result_after_move = any_cast<Value>(std::move(any));
        STATIC_REQUIRE(std::is_same_v<decltype(result_after_move), Value&&>);
        REQUIRE(result_after_move == Value{});
    }

    SECTION("get const&&")
    {
        const kiln::util::Any any{ std::in_place_type<Value>, value };

        [[maybe_unused]]
        decltype(auto) result = any_cast<Value>(std::move(any));

        STATIC_REQUIRE(std::is_same_v<decltype(result), const Value&&>);
        REQUIRE(result == value);
    }

    SECTION("large to small")
    {
        const kiln::util::Any large{ std::in_place_type<std::array<Value, 16>> };
        kiln::util::Any       small{ std::in_place_type<Value> };

        small = large;
    }

    SECTION("small to large")
    {
        const kiln::util::Any small{ std::in_place_type<Value> };
        kiln::util::Any       large{ std::in_place_type<std::array<Value, 16>> };

        large = small;
    }
}
