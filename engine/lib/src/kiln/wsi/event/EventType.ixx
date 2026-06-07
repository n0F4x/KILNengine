export module kiln.wsi.event.EventType;

namespace kiln::wsi {

export enum struct EventType
{
    eWindowCloseRequestedEvent,
    eFramebufferResizedEvent,
    eKeyPressedEvent,
    eKeyReleasedEvent,
    eCursorMovedEvent,
};

}   // namespace kiln::wsi
