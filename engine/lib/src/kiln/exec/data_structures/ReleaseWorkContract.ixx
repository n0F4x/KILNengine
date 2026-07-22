export module kiln.exec.data_structures.ReleaseWorkContract;

import kiln.util.containers.MoveOnlyFunction;

namespace kiln::exec {

export using ReleaseWorkContract = util::MoveOnlyFunction<void()>;

}   // namespace kiln::exec
