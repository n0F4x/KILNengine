export module kiln.app.registry.CyclicDependencyDetected;

import kiln.util.contracts;

namespace kiln::app {

export class CyclicDependencyDetected
    : public util::PreconditionViolation   //
{
    using PreconditionViolation::PreconditionViolation;
};

}   // namespace kiln::app
