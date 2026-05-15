module;

#include <concepts>
#include <optional>
#include <type_traits>

export module kiln.util.Bool;

namespace kiln::util {

export class Bool {
public:
    explicit Bool() = default;
    constexpr explicit Bool(bool value) noexcept;

    constexpr auto operator=(bool value) noexcept -> Bool&;

    constexpr explicit operator bool() const noexcept;

private:
    bool m_value{};
};

}   // namespace kiln::util

namespace kiln::util {

constexpr Bool::Bool(const bool value) noexcept : m_value{ value } {}

constexpr auto Bool::operator=(const bool value) noexcept -> Bool&
{
    m_value = value;
    return *this;
}

constexpr Bool::operator bool() const noexcept
{
    return m_value;
}

}   // namespace kiln::util
