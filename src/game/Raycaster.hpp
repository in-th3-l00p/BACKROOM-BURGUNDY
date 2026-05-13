#pragma once

#include "../app/Framebuffer.hpp"
#include "../app/Window.hpp"
#include "../patterns/WallShadingStrategy.hpp"

#include <iosfwd>
#include <memory>
#include <vector>

namespace escape::game {
    class Map;
    class Player;
    class Texture;

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
        auto depth_buffer() const noexcept -> const std::vector<float>& { return depth_buffer_; }

        auto cast_ray(int column, const Player& player, const Map& map) const -> RayHit;
        void render(const Player& player, const Map& map, app::Framebuffer& framebuffer);

        void set_floor_texture(std::shared_ptr<Texture> texture);
        void set_ceiling_texture(std::shared_ptr<Texture> texture);
        void set_shading_strategy(std::unique_ptr<patterns::WallShadingStrategy> strategy);

        auto shading_strategy() const noexcept -> const patterns::WallShadingStrategy& { return *shading_; }

    private:
        void render_walls(const Player& player, const Map& map, app::Framebuffer& framebuffer);
        void render_floor_and_ceiling(const Player& player, app::Framebuffer& framebuffer);

        int screen_width_;
        int screen_height_;
        std::vector<float> depth_buffer_;
        std::shared_ptr<Texture> floor_texture_;
        std::shared_ptr<Texture> ceiling_texture_;
        std::unique_ptr<patterns::WallShadingStrategy> shading_;
    };

    auto operator<<(std::ostream& stream, const Raycaster& raycaster) -> std::ostream&;
}
