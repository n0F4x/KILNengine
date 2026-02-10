module;

#include <format>
#include <source_location>
#include <string>
#include <string_view>

export module kiln.util.contracts;

namespace kiln::util {

export class PreconditionViolation {
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

export constexpr auto print_precondition_message_and_break(
    const std::string_view      condition_as_string,
    const std::source_location& location,
    const std::string_view      message = ""
) -> void
{
    const PreconditionViolation precondition_violation{
        location,
        std::format(
            "`{}`{}",
            condition_as_string,
            message.empty() ? "" : std::format(", \"{}\"", message)
        )
    };

#ifdef KILN_TEST
    throw precondition_violation;
#endif

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

}   // namespace kiln::util
