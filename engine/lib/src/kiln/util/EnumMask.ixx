module;

#include <type_traits>
#include <utility>

export module kiln.util.EnumMask;

namespace kiln::util {

export template <typename Enum_T>
    requires(std::is_enum_v<Enum_T>)
class EnumMask {
public:
    constexpr explicit(false) EnumMask(const Enum_T bit = Enum_T{})
        : m_mask{ std::to_underlying(bit) }
    {
    }

    [[nodiscard]]
    constexpr auto operator|=(const EnumMask other) noexcept -> EnumMask&
    {
        m_mask |= other.underlying();
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator&=(const EnumMask other) noexcept -> EnumMask&
    {
        m_mask &= other.underlying();
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator|(const EnumMask other) const noexcept -> EnumMask
    {
        return EnumMask{ m_mask | other.underlying() };
    }

    [[nodiscard]]
    constexpr auto operator&(const EnumMask other) const noexcept -> EnumMask
    {
        return EnumMask{ m_mask & other.underlying() };
    }

    [[nodiscard]]
    constexpr auto underlying() const noexcept -> std::underlying_type_t<Enum_T>
    {
        return m_mask;
    }

private:
    std::underlying_type_t<Enum_T> m_mask;
};

}   // namespace kiln::util
