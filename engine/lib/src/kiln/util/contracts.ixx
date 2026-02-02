module;

#include <cassert>
#include <format>
#include <source_location>
#include <string>
#include <string_view>

export module kiln.util.contracts;

namespace kiln::util {

namespace internal {

class PreconditionViolation {
public:
    constexpr PreconditionViolation(
        const std::source_location& location,
        const std::string_view      message
    )
        : m_location{ location },
          m_message{ message }
    {
    }

    auto print() const -> void;

private:
    std::source_location m_location;
    std::string          m_message;

    [[nodiscard]]
    constexpr static auto headline() -> std::string_view
    {
        constexpr static std::string_view result{ "Precondition violated!" };
        return result;
    }
};

}   // namespace internal

export constexpr auto print_precondition_message_and_break(
    const std::string_view      condition_as_string,
    const std::source_location& location
) -> void
{
    if consteval
    {
        assert(false);
    }
    else
    {
        const internal::PreconditionViolation precondition_violation{
            location, std::format("`{}`", condition_as_string)
        };
        precondition_violation.print();

        // TODO: std::breakpoint #p2546r5
#ifdef _MSC_VER
        __debugbreak();
#elifdef __clang__
        __builtin_debugtrap();
#else
        static_assert(false, "Compiler not supported");
#endif
    }
}

export constexpr auto print_precondition_message_and_break(
    const std::string_view      condition_as_string,
    const std::source_location& location,
    const std::string_view      message
) -> void
{
    if consteval
    {
        assert(false);
    }
    else
    {
        const internal::PreconditionViolation precondition_violation{
            location, std::format("`{}`, \"{}\"", condition_as_string, message)
        };
        precondition_violation.print();

        // TODO: std::breakpoint #p2546r5
#ifdef _MSC_VER
        __debugbreak();
#elifdef __clang__
        __builtin_debugtrap();
#else
        static_assert(false, "Compiler not supported");
#endif
    }
}

}   // namespace kiln::util
