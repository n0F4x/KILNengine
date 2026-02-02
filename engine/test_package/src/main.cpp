#include <iostream>

import kiln;

auto main() -> int
{
    std::cout << "Hello " << kiln::config::engine_name() << "!";
}
