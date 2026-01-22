#include <print>

#include "kiln/config/engine_name.hpp"

auto main() -> int
{
    std::println("Hello {}!", kiln::config::engine_name());
}
