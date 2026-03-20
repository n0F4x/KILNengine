module;

#include <optional>
#include <utility>

export module kiln.gfx.renderer.pipeline.DepthAttachment;

namespace kiln::gfx::renderer {

export class DepthAttachment {
public:
    [[nodiscard]]
    auto clear_value() const noexcept -> std::optional<float>;

    template <typename Self_T>
    auto set_clear_value(this Self_T&&, float value) noexcept -> Self_T&&;

private:
    std::optional<float> m_clear_value;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

inline auto DepthAttachment::clear_value() const noexcept -> std::optional<float>
{
    return m_clear_value;
}

template <typename Self_T>
auto DepthAttachment::set_clear_value(this Self_T&& self, const float value) noexcept
    -> Self_T&&
{
    self.DepthAttachment::m_clear_value = value;
    return std::forward<Self_T>(self);
}

}   // namespace kiln::gfx::renderer
