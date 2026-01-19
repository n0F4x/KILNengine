module;

#include <fmt/format.h>

export module kiln;

namespace kiln {

export auto hello() -> void
{
    fmt::println("Hello KILN!");
}

}   // namespace kiln
