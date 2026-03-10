export module kiln.wsi.Plugin;

import kiln.app.App;
import kiln.util.type_traits.const_like;
import kiln.wsi.Context;

namespace kiln::wsi {

export class Plugin {
public:
    template <typename Self_T>
    [[nodiscard]]
    auto context(this Self_T& self) -> util::const_like_t<Context, Self_T>
    {
        return self.Plugin::m_context;
    }

    auto operator()(app::App& app) const -> void
    {
        app.context().insert(m_context);
    }

private:
    Context m_context;
};

}   // namespace kiln::wsi
