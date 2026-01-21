#pragma once

#include <utility>

#include "kiln/app/App.hpp"
#include "kiln/app/ContextBuilder.hpp"

namespace kiln::app {

template <typename T>
concept decays_to_resource_injection_c = decays_to_context_variable_injection_c<T>;

class Builder {
public:
    template <typename Self_T, decays_to_resource_injection_c Injection_T>
    auto inject_resource(this Self_T&&, Injection_T&& injection) -> Self_T;

    [[nodiscard]]
    auto build() && -> App;

private:
    ContextBuilder m_resource_context_builder;
};

[[nodiscard]]
auto create() -> Builder;

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T, decays_to_resource_injection_c Injection_T>
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
