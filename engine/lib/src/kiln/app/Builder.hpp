#pragma once

#include <utility>

#include "kiln/app/App.hpp"
#include "kiln/app/ContextBuilder.hpp"

namespace kiln::app {

class Builder {
public:
    [[nodiscard]]
    auto build() && -> App;

private:
    ContextBuilder m_resource_context_builder;
};

}   // namespace kiln::app

namespace kiln::app {

inline auto Builder::build() && -> App
{
    App result{};

    result.resources() = std::move(m_resource_context_builder).build();

    return result;
}

}   // namespace kiln::app
