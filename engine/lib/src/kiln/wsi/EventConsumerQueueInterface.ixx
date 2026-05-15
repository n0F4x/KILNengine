module;

#include <chrono>

export module kiln.wsi.EventConsumerQueueInterface;

import kiln.wsi.event.Event;

namespace kiln::wsi {

export class EventConsumerQueueInterface {
public:
    virtual ~EventConsumerQueueInterface() = default;

    virtual auto push(Event&& event, std::chrono::steady_clock::time_point time_point)
        -> void = 0;
};

}   // namespace kiln::wsi
