module;

#include <string_view>

export module kiln.res.resource_id_from_string;

import kiln.res.ResourceID;
import kiln.util.hash;

namespace kiln::res {

export [[nodiscard]]
constexpr auto resource_id_from_string(const std::string_view string) noexcept -> ResourceID
{
    return ResourceID{ util::hash_u64(string) };
}

}   // namespace kiln::resource
