module;

#include <GLFW/glfw3.h>

export module kiln.wsi.event.wait_events;

import kiln.wsi.Context;

namespace kiln::wsi {

export auto wait_events(const Context&) -> void
{
    glfwWaitEvents();
}

}   // namespace kiln::wsi
