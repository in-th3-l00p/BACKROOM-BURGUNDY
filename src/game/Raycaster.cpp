#include "Raycaster.hpp"

#include "Map.hpp"
#include "Player.hpp"
#include "Texture.hpp"
#include "Tile.hpp"

#include <algorithm>
#include <cmath>
#include <ostream>

namespace escape::game {
    Raycaster::Raycaster(int screen_width, int screen_height)
        : screen_width_(screen_width),
          screen_height_(screen_height),
          depth_buffer_(static_cast<std::size_t>(screen_width), 1e30F),
          shading_(std::make_unique<patterns::LinearDistanceShading>(20.0F)) {}

    void Raycaster::set_floor_texture(std::shared_ptr<Texture> texture) {
        floor_texture_ = std::move(texture);
    }

    void Raycaster::set_ceiling_texture(std::shared_ptr<Texture> texture) {
        ceiling_texture_ = std::move(texture);
    }

    void Raycaster::set_shading_strategy(std::unique_ptr<patterns::WallShadingStrategy> strategy) {
        if (strategy != nullptr) {
            shading_ = std::move(strategy);
        }
    }

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

    void Raycaster::render_floor_and_ceiling(const Player& player, app::Framebuffer& framebuffer) {
        const int width = screen_width_;
        const int height = screen_height_;
        const float pos_x = player.position().x();
        const float pos_y = player.position().y();
        const float dir_x = player.direction().x();
        const float dir_y = player.direction().y();
        const float plane_x = player.camera_plane().x();
        const float plane_y = player.camera_plane().y();

        const float ray_dir_x_0 = dir_x - plane_x;
        const float ray_dir_y_0 = dir_y - plane_y;
        const float ray_dir_x_1 = dir_x + plane_x;
        const float ray_dir_y_1 = dir_y + plane_y;

        const float pos_z = 0.5F * static_cast<float>(height);

        for (int y = 0; y < height; ++y) {
            const bool is_floor = y > height / 2;
            const int p = is_floor ? (y - height / 2) : (height / 2 - y);
            if (p == 0) {
                continue;
            }
            const float row_distance = pos_z / static_cast<float>(p);

            const float floor_step_x = row_distance * (ray_dir_x_1 - ray_dir_x_0) / static_cast<float>(width);
            const float floor_step_y = row_distance * (ray_dir_y_1 - ray_dir_y_0) / static_cast<float>(width);
            float floor_x = pos_x + row_distance * ray_dir_x_0;
            float floor_y = pos_y + row_distance * ray_dir_y_0;

            for (int x = 0; x < width; ++x) {
                const int cell_x = static_cast<int>(floor_x);
                const int cell_y = static_cast<int>(floor_y);
                const float u = floor_x - static_cast<float>(cell_x);
                const float v = floor_y - static_cast<float>(cell_y);

                if (is_floor && floor_texture_ != nullptr) {
                    framebuffer.set_pixel(x, y, floor_texture_->sample_normalized(u, v));
                } else if (!is_floor && ceiling_texture_ != nullptr) {
                    framebuffer.set_pixel(x, y, ceiling_texture_->sample_normalized(u, v));
                } else if (is_floor) {
                    framebuffer.set_pixel(x, y, app::Color {.r = 40, .g = 40, .b = 40, .a = 255});
                } else {
                    framebuffer.set_pixel(x, y, app::Color {.r = 30, .g = 30, .b = 50, .a = 255});
                }

                floor_x += floor_step_x;
                floor_y += floor_step_y;
            }
        }
    }

    void Raycaster::render_walls(const Player& player, const Map& map, app::Framebuffer& framebuffer) {
        for (int x = 0; x < screen_width_; ++x) {
            const auto hit = cast_ray(x, player, map);
            const float safe_distance = std::max(hit.perp_distance, 0.0001F);
            depth_buffer_[static_cast<std::size_t>(x)] = safe_distance;

            const int line_height = static_cast<int>(static_cast<float>(screen_height_) / safe_distance);
            const int unclipped_top = -line_height / 2 + screen_height_ / 2;
            int draw_start = unclipped_top;
            int draw_end = line_height / 2 + screen_height_ / 2;
            draw_start = std::max(draw_start, 0);
            draw_end = std::min(draw_end, screen_height_ - 1);

            if (!map.in_bounds(hit.map_x, hit.map_y)) {
                continue;
            }
            const auto& tile = map.tile_at(hit.map_x, hit.map_y);
            for (int y = draw_start; y <= draw_end; ++y) {
                const float wall_v = static_cast<float>(y - unclipped_top)
                                   / static_cast<float>(std::max(line_height, 1));
                const auto base = tile.sample(hit.wall_x, wall_v, hit.side);
                if (base.a == 0) {
                    continue;
                }
                framebuffer.set_pixel(x, y, shading_->apply(base, safe_distance));
            }
        }
    }

    void Raycaster::render(const Player& player, const Map& map, app::Framebuffer& framebuffer) {
        framebuffer.clear(app::Color {.r = 0, .g = 0, .b = 0, .a = 255});
        render_floor_and_ceiling(player, framebuffer);
        render_walls(player, map, framebuffer);
    }

    auto operator<<(std::ostream& stream, const Raycaster& raycaster) -> std::ostream& {
        stream << "Raycaster{width=" << raycaster.screen_width()
               << ", height=" << raycaster.screen_height() << "}";
        return stream;
    }
}
