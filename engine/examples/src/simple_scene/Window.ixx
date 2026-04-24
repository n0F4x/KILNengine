export module examples.simple_scene.Window;

import kiln.app.config.Config;
import kiln.app.context.ContextBuilderInterface;
import kiln.wsi.Context;
import kiln.wsi.Window;

namespace demo {

export class Window {
public:
    class Builder;

    explicit Window(const kiln::app::Config& config, const kiln::wsi::Context& context);

    [[nodiscard]]
    auto operator*() -> kiln::wsi::Window&;
    [[nodiscard]]
    auto operator*() const -> const kiln::wsi::Window&;
    auto operator->() -> kiln::wsi::Window*;
    auto operator->() const -> const kiln::wsi::Window*;

private:
    kiln::wsi::Window m_window;
};

class Window::Builder : public kiln::app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto build(const kiln::app::Config& config, const kiln::wsi::Context& context)
        -> Window;
};

}   // namespace demo
