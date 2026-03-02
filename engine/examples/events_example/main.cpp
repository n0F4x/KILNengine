#include <print>

import kiln.app;
import kiln.event;

struct EventTest {
    int a;
    int b;
};

auto event_test(const EventTest e) -> void
{
    std::println("Event test, sum:{}", e.a + e.b);
}

auto event_test2(const EventTest e) -> void
{
    std::println("Event test2, diff:{}", e.a - e.b);
}

auto main() -> int
{
    using namespace kiln;

    // builder needs to be configured on the stack, otherwise we run into a bug on Windows
    app::Builder builder = app::create()
                                .insert_plugin(event::Plugin{});
    app::App app = std::move(builder).build();

    event::EventSystem& event_system = app.resources().at<event::EventSystem>();

    const auto id1 =
        event_system.subscribe<EventTest>(event_test);
    const auto id2 =
        event_system.subscribe<EventTest>(event_test2, 1);

    event_system.publish<EventTest>({3, 2});
    event_system.unsubscribe(id1);
    event_system.publish<EventTest>({4, 1});
    event_system.unsubscribe(id2);
}