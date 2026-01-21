#pragma once

namespace kiln::util {

template <typename>
struct always_true {
    constexpr static bool value = true;
};

}   // namespace kiln::util
