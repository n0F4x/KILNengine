export module kiln.wsi.event.EventType;

namespace kiln::wsi {

export enum struct EventType
{
    eWindowCloseRequestedEvent,
    eFramebufferResizedEvent,
    eCursorMovedEvent,
};

}   // namespace kiln::wsi
