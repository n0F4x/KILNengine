module;

#include <expected>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.wsi.WindowProxy;

import kiln.util.contracts;
import kiln.wsi.window_functions;

namespace kiln::wsi {

WindowProxy::WindowProxy(WindowProxy&& other, const allocator_type& allocator)
    : m_context{ std::move(other.m_context) },
      m_handle{ other.m_handle },
      m_changes{ std::move(other.m_changes), allocator }
{
}

WindowProxy::WindowProxy(const Context& context, const CreateInfo& create_info)
    : WindowProxy{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          context,
          create_info,
      }
{
}

WindowProxy::WindowProxy(
    std::allocator_arg_t,
    const allocator_type& allocator,
    const Context&        context,
    const CreateInfo&     create_info
)
    : m_context{ context },
      m_handle{ create_window(m_context, create_info.title, create_info.settings) },
      m_changes{ allocator }
{
}

auto WindowProxy::get_allocator() const noexcept -> allocator_type
{
    return m_changes.get_allocator();
}

auto WindowProxy::context() const noexcept -> const Context&
{
    return m_context;
}

auto WindowProxy::destroy() -> void
{
    if (!m_destroyed)
    {
        m_changes.emplace_back(
            m_context,
            [context = m_context, handle = m_handle] -> void
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
    return wsi::create_vulkan_surface(m_context, m_handle, instance);
}

auto WindowProxy::flush_changes(
    const std::pmr::vector<WindowCommand>::allocator_type& allocator
) -> std::pmr::vector<WindowCommand>
{
    std::pmr::vector result{ std::move(m_changes), allocator };
    m_changes.clear();
    return result;
}

}   // namespace kiln::wsi
