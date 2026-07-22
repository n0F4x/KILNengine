export module kiln.exec.data_structures.WorkContinuation;

namespace kiln::exec {

export enum struct WorkContinuation {
    eDontCare,
    eReschedule,
    eRelease,
};

}   // namespace kiln::exec
