#pragma once

#include "kiln/app/App.hpp"
#include "kiln/util/GenericStackBuilder.hpp"

namespace kiln::app {

class ResourcePlugin {
public:
    template <typename Self_T, util::decays_to_generic_stack_item_injection_c Injection_T>
    auto inject_resource(this Self_T&&, Injection_T&& injection) -> Self_T;

    auto operator()(App& app) && -> void;

private:
    util::GenericStackBuilder m_resource_stack_builder;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, util::decays_to_generic_stack_item_injection_c Injection_T>
auto ResourcePlugin::inject_resource(this Self_T&& self, Injection_T&& injection)
    -> Self_T
{
    self.ResourcePlugin::m_resource_stack_builder.inject(std::forward<Injection_T>(injection));
    return std::forward<Self_T>(self);
}

inline auto ResourcePlugin::operator()(App& app) && -> void
{
    PRECOND(
        app.resources().empty(),
        "Resource plugin should be called before adding any resource"
    );

    app.resources() = std::move(m_resource_stack_builder).build();
}

}   // namespace kiln::app
