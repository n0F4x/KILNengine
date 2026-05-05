module;

#include <GLFW/glfw3.h>

export module kiln.wsi.event.poll_events;

import kiln.wsi.Context;

namespace kiln::wsi {

export auto poll_events(const Context&) -> void
{
    glfwPollEvents();
}

}   // namespace kiln::wsi
