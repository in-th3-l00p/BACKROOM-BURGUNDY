#pragma once

#include "../app/Window.hpp"

#include <iosfwd>

namespace escape::game {
    class RenderStyle {
    public:
        explicit RenderStyle(app::Color fill_color);

        auto fill_color() const noexcept -> app::Color;
        auto with_alpha(unsigned char alpha) const -> RenderStyle;

    private:
        app::Color fill_color_ {};
    };

    auto operator<<(std::ostream& stream, const RenderStyle& style) -> std::ostream&;
} 
