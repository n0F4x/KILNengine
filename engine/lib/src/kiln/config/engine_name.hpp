#pragma once

#include "kiln/util/StringLiteral.hpp"

namespace kiln::config {

[[nodiscard]]
consteval auto engine_name() noexcept -> util::StringLiteral
{
    return "KILN_ENGINE";
}

}   // namespace kiln::config
