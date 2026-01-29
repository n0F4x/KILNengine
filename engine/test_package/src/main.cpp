#include <iostream>

#include <kiln.hpp>

auto main() -> int
{
    std::cout << "Hello " << kiln::config::engine_name() << "!";
}
