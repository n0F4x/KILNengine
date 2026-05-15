module;

#include <chrono>
#include <format>

#include <GLFW/glfw3.h>

#include "kiln/util/contract_macros.hpp"

module kiln.wsi.Engine;

import kiln.util.contracts;
import kiln.util.reflection;
import kiln.wsi.event.Event;
import kiln.wsi.event.events;
import kiln.wsi.event.EventType;
import kiln.wsi.event.poll_events;
import kiln.wsi.event.wait_events;
import kiln.wsi.window_functions;

namespace kiln::wsi {

EnginePrecondition::EnginePrecondition()
{
    PRECOND(
        instance_count == 0,
        std::format("Only one {} must be alive at a time", util::name_of<Engine>())
    );
    ++instance_count;
}

EnginePrecondition::~EnginePrecondition()
{
    --instance_count;
}

uint32_t EnginePrecondition::instance_count{};

Engine::Engine(EventConsumerQueueInterface& event_consume_queue)
    : m_event_consume_queue_ref{ event_consume_queue }
{
}

auto Engine::context() const noexcept -> const Context&
{
    return m_context;
}

auto Engine::create_window(const char* const title, const WindowSettings& settings) const
    -> WindowHandle
{
    const WindowHandle result{ wsi::create_window(m_context, title, settings) };

    set_window_user_pointer(m_context, result, &m_event_consume_queue_ref.get());
    set_callbacks(result);

    return result;
}

auto Engine::poll_events() const -> void
{
    wsi::poll_events(m_context);
}

auto Engine::wait_events() const -> void
{
    wsi::wait_events(m_context);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto Engine::post_empty_event() const -> void
{
    glfwPostEmptyEvent();
}

auto set_window_close_requested_callback(const WindowHandle window) -> void
{
    PRECOND(window != nullptr);

    glfwSetWindowCloseCallback(
        window.get(),
        +[](GLFWwindow* const raw_window) -> void
        {
            void* const user_pointer{ glfwGetWindowUserPointer(raw_window) };
            PRECOND(user_pointer != nullptr);

            EventConsumerQueueInterface& event_consume_queue{
                *static_cast<EventConsumerQueueInterface*>(user_pointer)
            };
            event_consume_queue.push(
                Event{
                    .type = EventType::eWindowCloseRequestedEvent,
                    .window_close_requested_event{ .window{ raw_window } },
                },
                std::chrono::steady_clock::now()
            );
        }
    );
}

auto set_framebuffer_size_callback(const WindowHandle window) -> void
{
    PRECOND(window != nullptr);

    glfwSetFramebufferSizeCallback(
        window.get(),
        +[](GLFWwindow* const raw_window, const int width, const int height) -> void
        {
            void* const user_pointer{ glfwGetWindowUserPointer(raw_window) };
            PRECOND(user_pointer != nullptr);

            EventConsumerQueueInterface& event_consume_queue{
                *static_cast<EventConsumerQueueInterface*>(user_pointer)
            };
            const FramebufferResizedEvent framebuffer_resized_event{
                .window{ raw_window },
                .new_size{ .width  = static_cast<uint32_t>(width),
                        .height = static_cast<uint32_t>(height) },
            };
            event_consume_queue.push(
                Event{
                    .type                      = EventType::eFramebufferResizedEvent,
                    .framebuffer_resized_event = framebuffer_resized_event,
                },
                std::chrono::steady_clock::now()
            );
        }
    );
}

auto set_cursor_pos_callback(const WindowHandle window) -> void
{
    PRECOND(window != nullptr);

    glfwSetCursorPosCallback(
        window.get(),
        +[](GLFWwindow* const raw_window, const double x, const double y) -> void
        {
            void* const user_pointer{ glfwGetWindowUserPointer(raw_window) };
            PRECOND(user_pointer != nullptr);

            EventConsumerQueueInterface& event_consume_queue{
                *static_cast<EventConsumerQueueInterface*>(user_pointer)
            };
            const CursorMovedEvent cursor_moved_event{
                .window = WindowHandle{ raw_window },
                .new_cursor_position{ .x = x, .y = y },
            };
            event_consume_queue.push(
                Event{
                    .type               = EventType::eCursorMovedEvent,
                    .cursor_moved_event = cursor_moved_event,
                },
                std::chrono::steady_clock::now()
            );
        }
    );
}

auto Engine::set_callbacks(const WindowHandle window) -> void
{
    set_window_close_requested_callback(window);
    set_framebuffer_size_callback(window);
    set_cursor_pos_callback(window);
}

}   // namespace kiln::wsi
