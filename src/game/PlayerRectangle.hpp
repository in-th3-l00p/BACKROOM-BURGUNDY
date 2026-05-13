#pragma once

#include "../app/Window.hpp"
#include "RenderStyle.hpp"
#include "Size2D.hpp"
#include "Vector2.hpp"

#include <iosfwd>
#include <string>

namespace escape::game {
    class PlayerRectangle {
    public:
        PlayerRectangle(std::string name, Vector2 position, Size2D size, RenderStyle style);
        PlayerRectangle(const PlayerRectangle& other);
        auto operator=(const PlayerRectangle& other) -> PlayerRectangle&;
        ~PlayerRectangle();

        auto name() const -> const std::string&;
        auto position() const -> const Vector2&;
        auto size() const -> const Size2D&;
        auto style() const -> const RenderStyle&;

        void move_with_direction(float axis_x, float axis_y, float speed, float delta_time_seconds);
        void clamp_to_bounds(const Size2D& bounds);
        auto to_window_rectangle() const -> app::Rectangle;

    private:
        std::string name_;
        Vector2 position_;
        Size2D size_;
        RenderStyle style_;
    };

    auto operator<<(std::ostream& stream, const PlayerRectangle& rectangle) -> std::ostream&;
}
