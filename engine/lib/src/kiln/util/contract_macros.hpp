#pragma once

#include <source_location>

#include "kiln/util/contract_details.hpp"

#define PRECOND(condition, ...)                                                            \
    ((void)((!!(condition))                                                                \
            || (::kiln::util::internal::print_precondition_message_and_break(              \
                    #condition, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__ \
                ),                                                                         \
                false)))
