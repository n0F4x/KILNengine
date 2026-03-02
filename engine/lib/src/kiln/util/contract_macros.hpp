#pragma once

#ifdef KILN_DEBUG
  #include <source_location>

  #define PRECOND(condition, ...)                                                \
      ((void)((!!(condition))                                                    \
              || (::kiln::util::print_precondition_message_and_break(            \
                      #condition,                                                \
                      std::source_location::current() __VA_OPT__(, ) __VA_ARGS__ \
                  ),                                                             \
                  false)))
#else
  #define PRECOND(condition, ...) ((void)0)   // NOLINT(*-macro-usage)
#endif
