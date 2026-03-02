module;

#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

export module kiln.event.EventPlugin;

import kiln.app.App;

namespace kiln::event {

using ListenerId = std::uint64_t;
using Priority = std::int64_t;

export class EventSystem {
private:
    struct HandlerEntry {
        ListenerId id;
        std::any callback;
        Priority priority;
    };

    std::unordered_map<std::type_index, std::vector<HandlerEntry>> m_event_map;
    ListenerId m_next_id = 1;

public:
    template <typename T>
    auto subscribe(std::function<void(const T&)> callback, const Priority priority = 0)
    -> ListenerId {
        const auto type_key = std::type_index(typeid(T));
        const ListenerId id = m_next_id++;

        auto& handlers = m_event_map[type_key];

        HandlerEntry entry{id, std::move(callback), priority};

        auto it = std::upper_bound(
            handlers.begin(),
            handlers.end(),
            entry,
            [](const HandlerEntry& a, const HandlerEntry& b) -> auto {
                return a.priority > b.priority;  // higher priority first
            });

        handlers.insert(it, std::move(entry));
        return id;
    };

    template <typename T>
    auto unsubscribe(ListenerId id) -> void {
        const auto type_key = std::type_index(typeid(T));
        if (!m_event_map.contains(type_key)) { return; }

        auto& handlers = m_event_map[type_key];
        // Remove the handler with the matching ID
        std::erase_if(handlers, [id](const HandlerEntry& entry) -> auto {
            return entry.id == id;
        });
    }

    template <typename T, typename... Args_T>
    auto publish(Args_T&&... args) -> void {
        const auto type_key = std::type_index(typeid(T));
        const auto it = m_event_map.find(type_key);
        if (it == m_event_map.end()) { return; }

        T event_instance(std::forward<Args_T>(args)...);

        for (auto& entry : it->second) {
            // Cast back to the specific function type
            auto& func = std::any_cast<std::function<void(const T&)>&>(entry.callback);
            func(event_instance);
        }
    };
};

export class EventPlugin {
public:
    auto operator()(app::App& app) const -> void {
        EventSystem events;
        app.resources().insert(
            events
        );
    }
};

}

