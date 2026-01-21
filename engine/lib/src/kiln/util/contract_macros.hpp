#pragma once

#include <source_location>

#include "kiln/util/contracts.hpp"

#define PRECOND(condition, ...)                                                           \
    ::kiln::util::assert_precondition(                                                      \
        condition, #condition, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__ \
    )
