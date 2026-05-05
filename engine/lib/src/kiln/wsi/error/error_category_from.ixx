module;

#include <cassert>
#include <utility>

#include <GLFW/glfw3.h>

export module kiln.wsi.error.error_category_from;

import kiln.wsi.error.ErrorCategory;

namespace kiln::wsi {

export [[nodiscard]]
auto error_category_from(const int error_code) -> ErrorCategory
{
    switch (error_code)
    {
        case GLFW_NO_ERROR:   //
            return ErrorCategory::eSuccess;
        case GLFW_API_UNAVAILABLE:
        case GLFW_VERSION_UNAVAILABLE:
        case GLFW_FORMAT_UNAVAILABLE:
        case GLFW_CURSOR_UNAVAILABLE:
        case GLFW_FEATURE_UNAVAILABLE:
        case GLFW_FEATURE_UNIMPLEMENTED:
        case GLFW_PLATFORM_UNAVAILABLE:   //
            return ErrorCategory::eRuntimeError;
        case GLFW_NOT_INITIALIZED:
        case GLFW_NO_CURRENT_CONTEXT:
        case GLFW_INVALID_ENUM:
        case GLFW_INVALID_VALUE:
        case GLFW_NO_WINDOW_CONTEXT:   //
            return ErrorCategory::ePreconditionViolationError;
        case GLFW_OUT_OF_MEMORY:
        case GLFW_PLATFORM_ERROR:   //
            return ErrorCategory::eInternalContractViolationError;
        default: assert(false && "Unhandled error code"); std::unreachable();
    }
}

}   // namespace kiln::wsi
