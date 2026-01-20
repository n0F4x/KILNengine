module;

#include <format>
#include <source_location>
#include <string>
#include <string_view>

#include <fmt/color.h>
#include <fmt/compile.h>

export module kiln.util.contracts;

namespace kiln::util {

export constexpr auto assert_precondition(
    bool                        condition,
    std::string_view            condition_as_string,
    const std::source_location& location
) -> void;

export constexpr auto assert_precondition(
    bool                        condition,
    std::string_view            condition_as_string,
    const std::source_location& location,
    std::string_view            message
) -> void;

}   // namespace kiln::util

namespace kiln::util {

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

    auto print() const -> void
    {
        fmt::println("{}", fmt::styled(headline(), fmt::fg(fmt::color::orange_red)));
        fmt::println(
            "    {} {} ({}:{})",
            fmt::styled("source:", fmt::fg(fmt::color::spring_green)),
            m_location.file_name(),
            m_location.line(),
            m_location.column()
        );
        fmt::println(
            "    {} {}",
            fmt::styled("function:", fmt::fg(fmt::color::yellow_green)),
            m_location.function_name()
        );
        fmt::println(
            "    {} {}",
            fmt::styled("message:", fmt::fg(fmt::color::cornflower_blue)),
            fmt::styled(m_message, fmt::emphasis::bold)
        );
    }

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

constexpr auto assert_precondition(
    const bool                  condition,
    const std::string_view      condition_as_string,
    const std::source_location& location
) -> void
{
    if (!condition) [[unlikely]]
    {
        const PreconditionViolation precondition_violation{
            location, std::format("`{}`", condition_as_string)
        };
        precondition_violation.print();
        // TODO: stop debugger if present #p2546r5
        __debugbreak();
    }
}

constexpr auto assert_precondition(
    const bool                  condition,
    const std::string_view      condition_as_string,
    const std::source_location& location,
    const std::string_view      message
) -> void
{
    if (!condition) [[unlikely]]
    {
        const PreconditionViolation precondition_violation{
            location, std::format("`{}`, \"{}\"", condition_as_string, message)
        };
        precondition_violation.print();
        // TODO: stop debugger if present #p2546r5
        __debugbreak();
    }
}

}   // namespace kiln::util
