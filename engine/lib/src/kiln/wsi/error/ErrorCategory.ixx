export module kiln.wsi.error.ErrorCategory;

namespace kiln::wsi {

export enum struct ErrorCategory
{
    eSuccess,
    eRuntimeError,
    ePreconditionViolationError,
    eInternalContractViolationError,
};

}   // namespace kiln::wsi
