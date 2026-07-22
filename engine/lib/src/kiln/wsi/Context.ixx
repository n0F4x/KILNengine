module;

#include <GLFW/glfw3.h>

export module kiln.wsi.Context;

import kiln.reg.EntryTraits;
import kiln.util.type_traits.const_like;
import kiln.wsi.error.handle_glfw_error;

namespace kiln::wsi {

export class Context {
public:
    Context();
    Context(const Context&);
    Context(Context&&) noexcept;
    ~Context();

    auto operator=(const Context&) -> Context&     = default;
    auto operator=(Context&&) noexcept -> Context& = default;

private:
    inline static uint32_t active_context_count{};
};

}   // namespace kiln::wsi

template <>
struct kiln::reg::EntryTraits<kiln::wsi::Context> {
    constexpr static bool is_configuration_entry{ true };
};

module :private;

namespace kiln::wsi {

auto initialize() -> void
{
    glfwSetErrorCallback(handle_glfw_error);
    glfwInit();
}

Context::Context()
{
    if (active_context_count == 0)
    {
        initialize();
    }

    ++active_context_count;
}

Context::Context(const Context&)
{
    ++active_context_count;
}

Context::Context(Context&&) noexcept
{
    ++active_context_count;
}

Context::~Context()
{
    --active_context_count;

    if (active_context_count == 0)
    {
        glfwTerminate();
    }
}

}   // namespace kiln::wsi
