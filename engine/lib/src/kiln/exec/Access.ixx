module;

#include <cstdint>

export module kiln.exec.Access;

import kiln.exec.AccessPattern;

namespace kiln::exec {

export struct Access {
    AccessPattern access_pattern;
    uint64_t      type_hash;
};

}   // namespace kiln::exec
