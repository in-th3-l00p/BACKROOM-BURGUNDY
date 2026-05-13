#include "RenderStyle.hpp"

#include <ostream>

namespace escape::game {
    RenderStyle::RenderStyle(app::Color fill_color)
        : fill_color_(fill_color) {}

    auto RenderStyle::fill_color() const noexcept -> app::Color {
        return fill_color_;
    }

    auto RenderStyle::with_alpha(unsigned char alpha) const -> RenderStyle {
        auto color = fill_color_;
        color.a = alpha;
        return RenderStyle {color};
    }

    auto operator<<(std::ostream& stream, const RenderStyle& style) -> std::ostream& {
        const auto color = style.fill_color();
        stream << "RenderStyle{fill_color=Color{r=" << static_cast<int>(color.r)
            << ", g=" << static_cast<int>(color.g)
            << ", b=" << static_cast<int>(color.b)
            << ", a=" << static_cast<int>(color.a) << "}}";
        return stream;
    }
}
