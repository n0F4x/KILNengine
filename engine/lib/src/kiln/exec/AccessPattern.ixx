module;

#include <algorithm>
#include <utility>

export module kiln.exec.AccessPattern;

namespace kiln::exec {

export enum struct AccessPattern
{
    eRead,
    eSharedWrite,
    eWrite,
};

export [[nodiscard]]
constexpr auto operator|(const AccessPattern& lhs, const AccessPattern& rhs)
    -> AccessPattern
{
    return AccessPattern{ std::max(std::to_underlying(lhs), std::to_underlying(rhs)) };
}

export constexpr auto operator|=(AccessPattern& lhs, const AccessPattern& rhs)
    -> AccessPattern&
{
    return lhs
         = AccessPattern{ std::max(std::to_underlying(lhs), std::to_underlying(rhs)) };
}

}   // namespace kiln::exec
