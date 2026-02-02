export module kiln.app.create;

import kiln.app.Builder;

namespace kiln::app {

export [[nodiscard]]
inline auto create() -> Builder
{
    return Builder{};
}

}   // namespace kiln::app
