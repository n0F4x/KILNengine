module;

#include <fmt/format.h>

export module kiln;

export import kiln.util;

namespace kiln {

export auto hello() -> void
{
    fmt::println("Hello KILN!");
}

}   // namespace kiln
