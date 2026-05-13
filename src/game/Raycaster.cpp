#include "Raycaster.hpp"

#include "Map.hpp"
#include "Player.hpp"

#include <algorithm>
#include <cmath>
#include <ostream>

namespace escape::game {
    Raycaster::Raycaster(int screen_width, int screen_height)
        : screen_width_(screen_width), screen_height_(screen_height) {}

    auto Raycaster::cast_ray(int column, const Player& player, const Map& map) const -> RayHit {
        const auto camera_x = 2.0F * static_cast<float>(column) / static_cast<float>(screen_width_) - 1.0F;
        const auto ray_dir_x = player.direction().x() + player.camera_plane().x() * camera_x;
        const auto ray_dir_y = player.direction().y() + player.camera_plane().y() * camera_x;

        int map_x = static_cast<int>(player.position().x());
        int map_y = static_cast<int>(player.position().y());

        const auto delta_dist_x = (ray_dir_x == 0.0F) ? 1e30F : std::fabs(1.0F / ray_dir_x);
        const auto delta_dist_y = (ray_dir_y == 0.0F) ? 1e30F : std::fabs(1.0F / ray_dir_y);

        int step_x = 0;
        int step_y = 0;
        float side_dist_x = 0.0F;
        float side_dist_y = 0.0F;

        if (ray_dir_x < 0.0F) {
            step_x = -1;
            side_dist_x = (player.position().x() - static_cast<float>(map_x)) * delta_dist_x;
        } else {
            step_x = 1;
            side_dist_x = (static_cast<float>(map_x) + 1.0F - player.position().x()) * delta_dist_x;
        }
        if (ray_dir_y < 0.0F) {
            step_y = -1;
            side_dist_y = (player.position().y() - static_cast<float>(map_y)) * delta_dist_y;
        } else {
            step_y = 1;
            side_dist_y = (static_cast<float>(map_y) + 1.0F - player.position().y()) * delta_dist_y;
        }

        int side = 0;
        int cell_value = 0;
        for (int safety = 0; safety < 1024; ++safety) {
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }
            cell_value = map.cell_at(map_x, map_y);
            if (cell_value > 0) {
                break;
            }
        }

        const float perp_distance = (side == 0)
            ? (side_dist_x - delta_dist_x)
            : (side_dist_y - delta_dist_y);

        float wall_x = (side == 0)
            ? (player.position().y() + perp_distance * ray_dir_y)
            : (player.position().x() + perp_distance * ray_dir_x);
        wall_x -= std::floor(wall_x);

        return RayHit {
            perp_distance,
            map_x,
            map_y,
            side,
            cell_value,
            wall_x,
            ray_dir_x,
            ray_dir_y,
        };
    }

    auto Raycaster::pick_wall_color(int cell_value, int side) const -> app::Color {
        auto color = app::Color {};
        switch (cell_value) {
        case 1: color = app::Color {.r = 220, .g = 50,  .b = 60,  .a = 255}; break;
        case 2: color = app::Color {.r = 50,  .g = 200, .b = 90,  .a = 255}; break;
        case 3: color = app::Color {.r = 70,  .g = 100, .b = 240, .a = 255}; break;
        case 4: color = app::Color {.r = 220, .g = 220, .b = 220, .a = 255}; break;
        case 5: color = app::Color {.r = 240, .g = 200, .b = 70,  .a = 255}; break;
        default: color = app::Color {.r = 200, .g = 200, .b = 200, .a = 255}; break;
        }
        if (side == 1) {
            color.r = static_cast<unsigned char>(color.r / 2);
            color.g = static_cast<unsigned char>(color.g / 2);
            color.b = static_cast<unsigned char>(color.b / 2);
        }
        return color;
    }

    void Raycaster::render(const Player& player, const Map& map, app::Window& window) const {
        const auto ceiling_color = app::Color {.r = 30, .g = 30, .b = 50, .a = 255};
        const auto floor_color   = app::Color {.r = 40, .g = 40, .b = 40, .a = 255};

        const int half_h = screen_height_ / 2;
        for (int x = 0; x < screen_width_; ++x) {
            window.draw_vertical_strip(x, 0, half_h, ceiling_color);
            window.draw_vertical_strip(x, half_h, screen_height_ - 1, floor_color);
        }

        for (int x = 0; x < screen_width_; ++x) {
            const auto hit = cast_ray(x, player, map);
            const float safe_distance = std::max(hit.perp_distance, 0.0001F);
            const int line_height = static_cast<int>(static_cast<float>(screen_height_) / safe_distance);

            int draw_start = -line_height / 2 + screen_height_ / 2;
            int draw_end = line_height / 2 + screen_height_ / 2;
            draw_start = std::max(draw_start, 0);
            draw_end = std::min(draw_end, screen_height_ - 1);

            const auto color = pick_wall_color(hit.cell_value, hit.side);
            window.draw_vertical_strip(x, draw_start, draw_end, color);
        }
    }

    auto operator<<(std::ostream& stream, const Raycaster& raycaster) -> std::ostream& {
        stream << "Raycaster{width=" << raycaster.screen_width()
               << ", height=" << raycaster.screen_height() << "}";
        return stream;
    }
}
