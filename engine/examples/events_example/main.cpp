#include <print>

import kiln.app;
import kiln.event;

struct EventTest {
    int a;
    int b;
};

auto event_test(const EventTest e) -> void
{
    std::println("Event test, sum:{}", e.a+e.b);
}
auto event_test2(const EventTest e) -> void
{
    std::println("Event test2, diff:{}", e.a-e.b);
}

auto main() -> int
{
    using namespace kiln;

    // builder needs to be configured on the stack, otherwise we run into a bug on Windows
    app::Builder builder = app::create()
                                .insert_plugin(event::EventPlugin{});
    app::App app = std::move(builder).build();

    const auto id1 =
        app.resources().at<event::EventSystem>()
                       .subscribe<EventTest>(event_test);
    const auto id2 =
        app.resources().at<event::EventSystem>()
                       .subscribe<EventTest>(event_test2, 1);
    app.resources().at<event::EventSystem>().publish<EventTest>(3, 2);
    app.resources().at<event::EventSystem>().unsubscribe<EventTest>(id1);
    app.resources().at<event::EventSystem>().publish<EventTest>(4, 1);
    app.resources().at<event::EventSystem>().unsubscribe<EventTest>(id2);
}