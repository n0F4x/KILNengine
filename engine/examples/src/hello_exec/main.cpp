#include <cstdio>

import kiln.app;
import kiln.exec;
import kiln.reg.Registry;

struct Message {
    const char* value;
};

auto greet(const kiln::exec::Ref<const Message> message) -> void
{
    std::puts(message->value);
}

auto main() -> int
{
    kiln::app::App app = kiln::app::create().apply_bundle(kiln::exec::Bundle{}).build();

    app.registry().insert(Message{ .value = "Hello exec!" });

    kiln::exec::Task task{ greet, app.registry() };

    task();

    return app.registry().contains<kiln::exec::WorkTree>();
}
