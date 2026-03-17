module;

#include <utility>

module kiln.app.plugin.PluginStack;

namespace kiln::app {

PluginStack::PluginStack(const allocator_type& allocator)
    : m_plugin_hashes{ allocator },
      m_plugins{ allocator }
{
}

PluginStack::PluginStack(PluginStack&& other, const allocator_type& allocator)
    : m_plugin_hashes{ std::move(other.m_plugin_hashes), allocator },
      m_plugins{ std::move(other.m_plugins), allocator }
{
}

PluginStack::~PluginStack()
{
    while (!m_plugins.empty())
    {
        m_plugins.pop_back();
    }
}

auto PluginStack::get_allocator() const -> allocator_type
{
    return m_plugins.get_allocator();
}

auto PluginStack::build(App& app) && -> void
{
    for (ErasedPlugin& plugin : m_plugins)
    {
        std::move(plugin).configure_and_build(app);
    }
}

}   // namespace kiln::app
