module;

#include <functional>
#include <memory>
#include <optional>
#include <variant>

#include <GLFW/glfw3.h>

#include "kiln/util/lifetimebound.hpp"

export module kiln.wsi.Window;

import kiln.wsi.Context;
import kiln.wsi.event.Key;
import kiln.wsi.Size;
import kiln.wsi.WindowSettings;
import kiln.util.StringLiteral;

namespace kiln::wsi {

export class Window {
public:
    struct CreateInfo {
        util::StringLiteral title{ "" };
        WindowSettings      settings;
    };

    Window(
        [[kiln_lifetimebound]]
        const Context&    context,
        const CreateInfo& create_info
    );

    [[nodiscard]]
    auto content_size() const noexcept -> Size2i;
    [[nodiscard]]
    auto resolution() const noexcept -> Size2u;

    [[nodiscard]]
    auto should_close() const noexcept -> bool;
    [[nodiscard]]
    auto key_pressed(Key key) const noexcept -> bool;

    auto request_close() noexcept -> void;

protected:
    [[nodiscard]]
    auto context() const noexcept -> const Context&;

    [[nodiscard]]
    auto handle() noexcept -> GLFWwindow&;
    [[nodiscard]]
    auto handle() const noexcept -> const GLFWwindow&;

private:
    std::reference_wrapper<const Context>                     m_context_ref;
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_handle;
};

}   // namespace kiln::wsi
