module;

#include <expected>
#include <utility>

#include "kiln/util/contract_macros.hpp"

module kiln.wsi.Window;

import kiln.util.contracts;
import kiln.wsi.event.KeyAction;
import kiln.wsi.window_functions;

namespace kiln::wsi {

Window::Window(Window&& other) noexcept
    : m_context{ std::move(other.m_context) },
      m_handle{ std::exchange(other.m_handle, nullptr) }
{
}

Window::~Window()
{
    reset();
}

Window::Window(const Context& context, const CreateInfo& create_info)
    : m_context{ context },
      m_handle{
          create_window(m_context, create_info.title, create_info.settings),
      }
{
}

auto Window::operator=(Window&& other) noexcept -> Window&
{
    if (this == &other)
    {
        return *this;
    }

    reset();

    m_handle = std::exchange(other.m_handle, nullptr);

    return *this;
}

auto Window::context() const noexcept -> const Context&
{
    return m_context;
}

auto Window::content_size() const noexcept -> Size2u
{
    PRECOND(m_handle != nullptr);
    return content_size_of(m_context, *m_handle);
}

auto Window::resolution() const noexcept -> Size2u
{
    PRECOND(m_handle != nullptr);
    return framebuffer_size_of(m_context, *m_handle);
}

auto Window::should_close() const noexcept -> bool
{
    PRECOND(m_handle != nullptr);
    return wsi::should_close(m_context, *m_handle);
}

// ReSharper disable once CppNotAllPathsReturnValue
auto Window::key_pressed(const Key key) const noexcept -> bool
{
    PRECOND(m_handle != nullptr);
    switch (get_key(m_context, *m_handle, key))
    {
        case KeyAction::eRelease: return false;
        case KeyAction::ePress:   return true;
        case KeyAction::eRepeat:  return true;
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Window::request_close() noexcept -> void
{
    PRECOND(m_handle != nullptr);
    set_should_close_flag(m_context, *m_handle, true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Window::create_vulkan_surface(const vk::raii::Instance& instance)
    -> std::expected<vk::raii::SurfaceKHR, vk::Result>
{
    PRECOND(m_handle != nullptr);
    return wsi::create_vulkan_surface(m_context, *m_handle, instance);
}

auto Window::reset() -> void
{
    if (m_handle != nullptr)
    {
        destroy_window(m_context, m_handle);
        m_handle = nullptr;
    }
}

}   // namespace kiln::wsi
