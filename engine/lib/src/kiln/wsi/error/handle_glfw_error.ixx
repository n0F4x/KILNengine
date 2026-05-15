module;

#include <cassert>
#include <utility>

#include "kiln/util/contract_macros.hpp"

export module kiln.wsi.error.handle_glfw_error;

import kiln.util.contracts;
import kiln.wsi.error.error_category_from;
import kiln.wsi.error.Error;
import kiln.wsi.error.ErrorCategory;

namespace kiln::wsi {

export auto handle_glfw_error(const int error_code, const char* description = nullptr)
    -> void
{
    switch (error_category_from(error_code))
    {
        case ErrorCategory::eSuccess: return;
        case ErrorCategory::eRuntimeError:
            throw Error{
                error_code,
                description == nullptr ? "" : description,
            };
        case ErrorCategory::ePreconditionViolationError:
            if (description == nullptr)
            {
                PRECOND(false);
            }
            else
            {
                PRECOND(false, description);
            }
            break;
        case ErrorCategory::eInternalContractViolationError: assert(false); break;
    }
    std::unreachable();
}

}   // namespace kiln::wsi
