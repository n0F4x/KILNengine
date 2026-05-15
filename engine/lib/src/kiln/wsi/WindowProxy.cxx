module;

#include <expected>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.wsi.WindowProxy;

import kiln.util.contracts;
import kiln.wsi.event.events;
import kiln.wsi.event.EventType;
import kiln.wsi.window_functions;

namespace kiln::wsi {

WindowProxy::WindowProxy(WindowProxy&& other, const allocator_type& allocator)
    : m_handle{ other.m_handle },
      m_cached_state{ other.m_cached_state },
      m_destroyed{ other.m_destroyed },
      m_changes{ std::move(other.m_changes), allocator }
{
}

WindowProxy::WindowProxy(const Context& context, const WindowHandle window_handle)
    : WindowProxy{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          context,
          window_handle,
      }
{
}

WindowProxy::WindowProxy(
    std::allocator_arg_t,
    const allocator_type& allocator,
    const Context&        context,
    const WindowHandle    window_handle
)
    : m_handle{ window_handle },
      m_cached_state{
          .close_flag       = wsi::should_close(context, m_handle),
          .framebuffer_size = framebuffer_size_of(context, m_handle),
          .cursor_position  = get_cursor_position(context, m_handle),
      },
      m_changes{ allocator }
{
    if (window_handle == nullptr)
    {
        m_destroyed = true;
    }
}

auto WindowProxy::operator==(const WindowHandle window_handle) const noexcept -> bool
{
    return m_handle == window_handle;
}

auto WindowProxy::get_allocator() const noexcept -> allocator_type
{
    return m_changes.get_allocator();
}

auto WindowProxy::number_of_pending_changes() const noexcept -> std::size_t
{
    return m_changes.size();
}

auto WindowProxy::should_close() const noexcept -> bool
{
    return m_cached_state.close_flag;
}

auto WindowProxy::framebuffer_size() const noexcept -> const Size2u&
{
    return m_cached_state.framebuffer_size;
}

auto WindowProxy::cursor_position() const noexcept -> const Position2d&
{
    return m_cached_state.cursor_position;
}

auto WindowProxy::destroy() -> void
{
    if (!m_destroyed)
    {
        m_changes.emplace_back(
            [handle = m_handle](const Context& context) -> void
            {
                destroy_window(context, handle);   //
            }
        );
        m_destroyed = true;
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto WindowProxy::create_vulkan_surface(const vk::raii::Instance& instance)
    -> std::expected<vk::raii::SurfaceKHR, vk::Result>
{
    PRECOND(!m_destroyed);
    return wsi::create_vulkan_surface(m_handle, instance);
}

auto WindowProxy::update(const Event& event) -> void
{
    switch (event.type)
    {
        using enum EventType;
        case eWindowCloseRequestedEvent:
        {
            if (event.window_close_requested_event.window == m_handle)
            {
                m_cached_state.close_flag = true;
            }
            break;
        }
        case eFramebufferResizedEvent:
        {
            if (event.framebuffer_resized_event.window == m_handle)
            {
                m_cached_state.framebuffer_size
                    = event.framebuffer_resized_event.new_size;
            }
            break;
        }
        case eCursorMovedEvent:
        {
            if (event.cursor_moved_event.window == m_handle)
            {
                m_cached_state.cursor_position
                    = event.cursor_moved_event.new_cursor_position;
            }
            break;
        }
    }
}

}   // namespace kiln::wsi
