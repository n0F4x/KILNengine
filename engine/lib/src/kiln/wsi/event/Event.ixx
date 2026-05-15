module;

export module kiln.wsi.event.Event;

import kiln.wsi.event.events;
import kiln.wsi.event.EventType;

namespace kiln::wsi {

export struct Event {
    EventType type;

    union {
        WindowCloseRequestedEvent window_close_requested_event;
        FramebufferResizedEvent   framebuffer_resized_event;
        CursorMovedEvent          cursor_moved_event;
    };
};

}   // namespace kiln::wsi
