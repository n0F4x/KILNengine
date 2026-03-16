module;

#include <concepts>
#include <vector>

export module kiln.app.plugin.PluginInterface;

import kiln.app.App;
import kiln.util.Function;

namespace kiln::app {

export class PluginInterface {
    using Configurator = util::MoveOnlyFunction<auto(PluginInterface&, App&) &&->void>;

public:
    template <typename Self_T, typename Configurator_T>
        requires std::invocable<std::decay_t<Configurator_T>&&, Self_T&, App&>
    auto register_configuration(this Self_T& self, Configurator_T&& configurator) -> void;

    template <typename Self_T>
    auto configure(this Self_T&&, App& app) -> Self_T&&;

private:
    std::vector<Configurator> m_configurators;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, typename Configurator_T>
    requires std::invocable<std::decay_t<Configurator_T>&&, Self_T&, App&>
auto PluginInterface::register_configuration(
    this Self_T&     self,
    Configurator_T&& configurator
) -> void
{
    self.PluginInterface::m_configurators.push_back(configurator);
}

template <typename Self_T>
auto PluginInterface::configure(this Self_T&& self, App& app) -> Self_T&&
{
    for (Configurator& configurator : self.PluginInterface::m_configurators)
    {
        std::move(configurator)(self, app);
    }

    return std::move(self);
}

}   // namespace kiln::app
