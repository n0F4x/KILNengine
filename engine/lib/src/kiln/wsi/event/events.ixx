export module kiln.wsi.event.events;

import kiln.wsi.Position;
import kiln.wsi.Size;
import kiln.wsi.WindowHandle;

namespace kiln::wsi {

export struct WindowCloseRequestedEvent {
    WindowHandle window;
};

export struct FramebufferResizedEvent {
    WindowHandle window;
    Size2u       new_size;
};

export struct CursorMovedEvent {
    WindowHandle window;
    Position2d   new_cursor_position;
};

}   // namespace kiln::wsi
