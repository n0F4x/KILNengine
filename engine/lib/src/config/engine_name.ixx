export module kiln.config.engine_name;

import kiln.util.StringLiteral;

namespace kiln::config {

export [[nodiscard]]
consteval auto engine_name() noexcept -> util::StringLiteral
{
    return "KILN_ENGINE";
}

}   // namespace kiln::config
