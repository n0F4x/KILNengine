module;

#include <memory>

export module kiln.res.WeakResourceHandle;

import kiln.res.resource_c;

namespace kiln::res {

export template <resource_c Resource_T>
using WeakResourceHandle = std::weak_ptr<Resource_T>;

}   // namespace kiln::res
