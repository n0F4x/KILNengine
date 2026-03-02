module;

#include <cstdint>

export module kiln.event.SubscriberID;

import kiln.util.Strong;

namespace kiln::event {

export class SubscriberID : public util::Strong<uint32_t, SubscriberID> {
public:
    using Strong::Strong;
};

}   // namespace kiln::event
