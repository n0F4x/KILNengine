#pragma once

#include <utility>

#include "kiln/app/App.hpp"
#include "kiln/util/GenericStackBuilder.hpp"

namespace kiln::app {

class Builder {
public:
    template <typename Self_T, util::decays_to_generic_stack_item_injection_c Injection_T>
    auto inject_resource(this Self_T&&, Injection_T&& injection) -> Self_T;

    [[nodiscard]]
    auto build() && -> App;

private:
    util::GenericStackBuilder m_resource_context_builder;
};

[[nodiscard]]
auto create() -> Builder;

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, util::decays_to_generic_stack_item_injection_c Injection_T>
auto Builder::inject_resource(this Self_T&& self, Injection_T&& injection) -> Self_T
{
    self.Builder::m_resource_context_builder.inject(std::forward<Injection_T>(injection));
    return std::forward<Self_T>(self);
}

inline auto Builder::build() && -> App
{
    App result{};

    result.resources() = std::move(m_resource_context_builder).build();

    return result;
}

inline auto create() -> Builder
{
    return Builder{};
}

}   // namespace kiln::app
