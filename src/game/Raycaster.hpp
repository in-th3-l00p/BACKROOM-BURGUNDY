#pragma once

#include "../app/Window.hpp"

#include <iosfwd>

namespace escape::game {
    class Map;
    class Player;

    struct RayHit {
        float perp_distance {0.0F};
        int map_x {0};
        int map_y {0};
        int side {0};
        int cell_value {0};
        float wall_x {0.0F};
        float ray_dir_x {0.0F};
        float ray_dir_y {0.0F};
    };

    class Raycaster {
    public:
        Raycaster(int screen_width, int screen_height);

        auto screen_width() const noexcept -> int { return screen_width_; }
        auto screen_height() const noexcept -> int { return screen_height_; }

        auto cast_ray(int column, const Player& player, const Map& map) const -> RayHit;
        void render(const Player& player, const Map& map, app::Window& window) const;

    private:
        auto pick_wall_color(int cell_value, int side) const -> app::Color;

        int screen_width_;
        int screen_height_;
    };

    auto operator<<(std::ostream& stream, const Raycaster& raycaster) -> std::ostream&;
}
