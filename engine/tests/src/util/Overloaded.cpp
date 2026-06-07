#include <catch2/catch_test_macros.hpp>

import kiln.util.Overloaded;

namespace {

auto function() -> void {}

}   // namespace

TEST_CASE("util::Overloaded")
{
    SECTION("construct from lambda")
    {
        kiln::util::Overloaded{ [] {} };
    }

    SECTION("construct from function")
    {
        kiln::util::Overloaded{ function };
    }

    SECTION("construct from static lambda")
    {
        kiln::util::Overloaded{ [] static {} };
    }
}
