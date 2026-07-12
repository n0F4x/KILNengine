#include <print>
#include <thread>

import kiln.reg;
import kiln.exec;

auto say_hello_from_thread() -> void
{
    std::println("Hello from thread #{}", std::this_thread::get_id());
}

struct ContendedResource : kiln::reg::EntryBase {};

auto contend_for_resource_first(kiln::exec::Ref<ContendedResource>) -> void
{
    std::println(
        "first access to contended resource (Thread id #{})",
        std::this_thread::get_id()
    );
}

auto contend_for_resource_second(kiln::exec::Ref<ContendedResource>) -> void
{
    std::println(
        "second access to contended resource (Thread id #{})",
        std::this_thread::get_id()
    );
}

auto main() -> int
{
    kiln::reg::Registry registry;
    registry.insert(ContendedResource{});

    kiln::exec::TaskGroup task_group;
    task_group.push(kiln::exec::Task{ say_hello_from_thread, registry });
    task_group.push(kiln::exec::Task{ contend_for_resource_first, registry });
    task_group.push(kiln::exec::Task{ say_hello_from_thread, registry });
    task_group.push(kiln::exec::Task{ say_hello_from_thread, registry });
    task_group.push(kiln::exec::Task{ contend_for_resource_second, registry });
    task_group.push(kiln::exec::Task{ say_hello_from_thread, registry });
    kiln::exec::Task task{ std::move(task_group).build() };

    /*
     * The framework does not yet support parallelism.
     */
    task();
}
