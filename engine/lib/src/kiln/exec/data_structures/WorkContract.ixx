export module kiln.exec.data_structures.WorkContract;

import kiln.exec.data_structures.WorkContinuation;
import kiln.util.containers.MoveOnlyFunction;

namespace kiln::exec {

export using WorkContract = util::MoveOnlyFunction<WorkContinuation()>;

}   // namespace kiln::exec
