#include <cstdio>
#include <memory_resource>   // workaround for a linker bug

import kiln.reg.Registry;
import kiln.exec;

struct Message {
    const char* value;
};

auto greet(const kiln::exec::Ref<const Message> message) -> void
{
    std::puts(message->value);
}

auto main() -> int
{
    kiln::reg::Registry registry;
    registry.insert(Message{ .value = "Hello exec!" });

    kiln::exec::Task task{ greet, registry };

    task();
}
