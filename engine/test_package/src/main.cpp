#include <print>

import kiln;

auto main() -> int
{
    std::println("Hello {}!", kiln::config::engine_name());
}
