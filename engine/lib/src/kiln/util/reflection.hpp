#pragma once

#include <source_location>
#include <string_view>

namespace kiln::util {

template <typename T>
consteval auto name_of() noexcept -> std::string_view;

template <typename T>
consteval auto hash() noexcept -> uint64_t;
template <typename T>
consteval auto hash_u64() noexcept -> uint64_t;

}   // namespace kiln::util

namespace kiln::util {

namespace internal {

template <typename T>
[[nodiscard]]
consteval auto raw_pretty_function_name() noexcept -> std::string_view
{
    /* Clang:
     * std::string_view kiln::util::raw_pretty_function_name() [T = ...]
     *
     * GCC:
     * consteval std::string_view
     * kiln::util::raw_pretty_function_name()
     * [with auto T = ...; std::string_view = std::basic_string_view<char>]
     *
     * MSVC:
     * class std::basic_string_view<char,struct std::char_traits<char> >
     * __cdecl kiln::util::internal::raw_pretty_function_name<T...>(void) noexcept
     */
    return std::source_location::current().function_name();
}

[[nodiscard]]
consteval auto pretty_name_trailing_character() noexcept -> char
{
#if defined __clang__
    return ']';
#elif defined __GNUC__
    return ';'
#elif defined _MSC_VER
    return '>';
#else
    static_assert(false, "Compiler is not supported");
#endif
}

}   // namespace internal

template <typename T>
consteval auto name_of() noexcept -> std::string_view
{
    constexpr std::string_view pretty{ internal::raw_pretty_function_name<T>() };
    constexpr std::string_view leading_characters{
#ifdef __clang__
        "= "
#elifdef __GNUC__
        "= "
#elifdef _MSC_VER
        "kiln::util::internal::raw_pretty_function_name<"
#else
        [] [[noreturn]] static -> std::string_view {
            static_assert(false, "Compiler is not supported");
        }()
#endif
    };

    constexpr std::string_view::size_type trailing_offset =
        pretty.rfind(internal::pretty_name_trailing_character());
    static_assert(trailing_offset != std::string_view::npos);

    constexpr std::string_view::size_type offset =
        pretty.rfind(leading_characters, trailing_offset);
    static_assert(offset != std::string_view::npos);

    return pretty.substr(
        offset + leading_characters.length(),
        trailing_offset - (offset + leading_characters.length())
    );
}

template <typename T>
consteval auto hash() noexcept -> uint64_t
{
    return hash_u64<T>();
}

template <typename T>
consteval auto hash_u64() noexcept -> uint64_t
{   // "Fowler–Noll–Vo - 1a" hash function
    constexpr uint64_t offset{ 14'695'981'039'346'656'037ull };
    constexpr uint64_t prime{ 1'099'511'628'211ull };

    uint64_t result{ offset };

    for (const auto character : name_of<T>())
    {
        result = (result ^ static_cast<uint64_t>(character)) * prime;
    }

    return result;
}

}   // namespace kiln::util
