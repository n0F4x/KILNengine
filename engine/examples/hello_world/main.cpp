#include <print>

#include <kiln.hpp>

auto main() -> int
{
    std::println("Hello {}!", kiln::config::engine_name());
}
