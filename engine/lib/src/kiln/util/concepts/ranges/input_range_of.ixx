module;

#include <concepts>
#include <ranges>

export module kiln.util.concepts.ranges.input_range_of;

namespace kiln::util {

export template <typename Range_T, typename T>
concept input_range_of_c
    = std::ranges::input_range<Range_T>
   && std::convertible_to<std::ranges::range_reference_t<Range_T>, T>;

}   // namespace kiln::util
