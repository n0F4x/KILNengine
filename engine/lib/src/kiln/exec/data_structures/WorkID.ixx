module;

#include <cstdint>

export module kiln.exec.data_structures.WorkID;

import kiln.util.Strong;

namespace kiln::exec {

export using WorkID = util::Strong<uint64_t, struct WorkIDTag>;

}   // namespace kiln::exec
