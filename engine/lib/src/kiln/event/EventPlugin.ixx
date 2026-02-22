module;

#include <unordered_map>
#include <vector>
#include <any>
#include <functional>
#include <typeindex>

export module kiln.event.EventPlugin;

import kiln.app.App;

namespace kiln::event {

export class EventSystem {
private:
    std::unordered_map<std::type_index, std::any> m_eventMap;

public:
    template <typename T>
    auto subscribe(std::function<void(const T&)> callback) -> void {
        const auto type_key = std::type_index(typeid(T));

        if (m_eventMap.contains(type_key)) {
            m_eventMap[type_key] = std::vector<std::function<void(const T&)>>();
        }

        auto& handlers = std::any_cast<std::vector<std::function<void(const T&)>>&>(m_eventMap[type_key]);
        handlers.push_back(callback);
    };

    template <typename T, typename... Args_T>
    auto publish(Args_T&&... args) -> void {
        const auto type_key = std::type_index(typeid(T));
        if (!m_eventMap.contains(type_key))
        {
            return;
        }

        T event_instance(std::forward<Args_T>(args)...);

        auto& handlers = std::any_cast<std::vector<std::function<void(const T&)>>&>(m_eventMap[type_key]);
        for (const auto& handler : handlers) {
            handler(event_instance);
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

