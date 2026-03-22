module;

#include <cassert>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"

export module kiln.wsi.event.wait_events;

import kiln.wsi.Context;
import kiln.wsi.Error;
import kiln.util.contracts;

namespace kiln::wsi {

export auto wait_events(const Context&) -> void
{
    glfwWaitEvents();

    const char* error_description{};
    const int   error_code = glfwGetError(&error_description);
    if (error_code != GLFW_NO_ERROR)
    {
        PRECOND(error_code != GLFW_NOT_INITIALIZED);
        assert(error_code == GLFW_PLATFORM_ERROR && "Other error codes are unspecified");
        throw Error{ error_description };
    }
}

}   // namespace kiln::wsi
