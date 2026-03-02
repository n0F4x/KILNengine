module;

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <vector>

export module kiln.event.Plugin;

import kiln.app.App;
import kiln.event.event_c;
import kiln.event.SubscriberID;
import kiln.util.Any;
import kiln.util.Function;
import kiln.util.reflection;

namespace kiln::event {

export class EventSystem {
    struct HandlerEntry {
        SubscriberID      id;
        util::MoveOnlyAny subscriber;
        int32_t           priority;
    };

    std::unordered_map<uint64_t, std::vector<HandlerEntry>> m_event_map;
    std::unordered_map<SubscriberID, uint64_t>              m_id_event_type;
    uint32_t                                                m_next_id = 0;

public:
    template <event_c Event_T, typename Subscriber_T>
    auto subscribe(Subscriber_T&& subscriber, const int32_t priority = 0) -> SubscriberID
    {
        const uint64_t     type_key{ util::hash_u64<Event_T>() };
        const SubscriberID id{ m_next_id++ };

        auto& handlers = m_event_map[type_key];

        HandlerEntry entry{
            id,
            util::MoveOnlyAny{
                util::MoveOnlyFunction<void(const Event_T&)>{
                    std::forward<Subscriber_T>(subscriber)   //
                }   //
            },
            priority,
        };
        m_id_event_type[id] = type_key;

        const auto iter = std::ranges::upper_bound(
            handlers,
            entry,
            [](const HandlerEntry& a, const HandlerEntry& b) -> auto
            {
                return a.priority > b.priority;   // higher priority first
            }
        );

        handlers.insert(iter, std::move(entry));

        return SubscriberID{ id };
    }

    auto unsubscribe(const SubscriberID id) -> void
    {
        const auto type_key = m_id_event_type.at(id);
        const auto iter     = m_event_map.find(type_key);
        if (iter == m_event_map.end())
        {
            return;
        }

        std::erase_if(
            iter->second,
            [id](const HandlerEntry& entry) -> auto { return entry.id == id; }
        );
    }

    template <event_c Event_T>
    auto publish(const Event_T& event) -> void
    {
        const auto type_key = util::hash_u64<Event_T>();
        const auto iter     = m_event_map.find(type_key);
        if (iter == m_event_map.end())
        {
            return;
        }

        for (auto& entry : iter->second)
        {
            // Cast back to the specific function type
            using FuncType = util::MoveOnlyFunction<void(const Event_T&)>;
            auto& func     = util::any_cast<FuncType>(entry.subscriber);
            func(event);
        }
    }
};

export class Plugin {
public:
    auto operator()(app::App& app) const -> void
    {
        app.resources().emplace<EventSystem>();
    }
};

}   // namespace kiln::event
