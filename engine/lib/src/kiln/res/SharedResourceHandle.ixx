module;

#include <memory>
#include <type_traits>
#include <utility>

export module kiln.res.SharedResourceHandle;

import kiln.res.resource_c;

namespace kiln::res {

// TODO: use our own type instead of aliasing std::shared_ptr

export template <resource_c Resource_T>
using SharedResourceHandle = std::shared_ptr<Resource_T>;

export template <resource_c Resource_T, typename... Args_T>
    requires(std::is_constructible_v<Resource_T, Args_T && ...>)
auto make_shared_resource_handle(Args_T&&... args) -> SharedResourceHandle<Resource_T>
{
    return std::make_shared<Resource_T>(std::forward<Args_T>(args)...);
}

}   // namespace kiln::res
