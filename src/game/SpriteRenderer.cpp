#include "SpriteRenderer.hpp"

#include "Player.hpp"
#include "Raycaster.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"

#include <algorithm>
#include <cmath>
#include <ostream>

namespace escape::game {
    SpriteRenderer::SpriteRenderer(int screen_width, int screen_height)
        : screen_width_(screen_width), screen_height_(screen_height) {}

    void SpriteRenderer::render(const Player& player,
                                const std::vector<const Sprite*>& sprites,
                                const Raycaster& raycaster,
                                app::Framebuffer& framebuffer) const {
        const auto pos_x = player.position().x();
        const auto pos_y = player.position().y();
        const auto dir_x = player.direction().x();
        const auto dir_y = player.direction().y();
        const auto plane_x = player.camera_plane().x();
        const auto plane_y = player.camera_plane().y();

        auto ordered = sprites;
        std::sort(ordered.begin(), ordered.end(), [&](const Sprite* a, const Sprite* b) {
            const auto da = (a->position().x() - pos_x) * (a->position().x() - pos_x)
                          + (a->position().y() - pos_y) * (a->position().y() - pos_y);
            const auto db = (b->position().x() - pos_x) * (b->position().x() - pos_x)
                          + (b->position().y() - pos_y) * (b->position().y() - pos_y);
            return da > db;
        });

        const auto& depth = raycaster.depth_buffer();

        for (const auto* sprite : ordered) {
            const auto sprite_x = sprite->position().x() - pos_x;
            const auto sprite_y = sprite->position().y() - pos_y;

            const auto inv_det = 1.0F / (plane_x * dir_y - dir_x * plane_y);
            const auto transform_x = inv_det * (dir_y * sprite_x - dir_x * sprite_y);
            const auto transform_y = inv_det * (-plane_y * sprite_x + plane_x * sprite_y);

            if (transform_y <= 0.0F) {
                continue;
            }

            const int sprite_screen_x = static_cast<int>(
                (static_cast<float>(screen_width_) / 2.0F) * (1.0F + transform_x / transform_y));

            const int sprite_height = static_cast<int>(
                std::fabs(static_cast<float>(screen_height_) / transform_y) * sprite->scale());
            const int sprite_width = sprite_height;

            int draw_start_y = -sprite_height / 2 + screen_height_ / 2;
            int draw_end_y = sprite_height / 2 + screen_height_ / 2;
            const int unclipped_start_y = draw_start_y;
            draw_start_y = std::max(draw_start_y, 0);
            draw_end_y = std::min(draw_end_y, screen_height_ - 1);

            int draw_start_x = -sprite_width / 2 + sprite_screen_x;
            int draw_end_x = sprite_width / 2 + sprite_screen_x;
            const int unclipped_start_x = draw_start_x;
            draw_start_x = std::max(draw_start_x, 0);
            draw_end_x = std::min(draw_end_x, screen_width_ - 1);

            const auto& tex = sprite->texture();

            for (int stripe = draw_start_x; stripe <= draw_end_x; ++stripe) {
                if (transform_y >= depth[static_cast<std::size_t>(stripe)]) {
                    continue;
                }
                const int tex_x = static_cast<int>(
                    static_cast<float>((stripe - unclipped_start_x) * tex.width()) / static_cast<float>(std::max(sprite_width, 1)));

                for (int y = draw_start_y; y <= draw_end_y; ++y) {
                    const int tex_y = static_cast<int>(
                        static_cast<float>((y - unclipped_start_y) * tex.height()) / static_cast<float>(std::max(sprite_height, 1)));
                    const auto color = tex.sample(tex_x, tex_y);
                    if (color.a == 0) {
                        continue;
                    }
                    if (color.r == 0 && color.g == 0 && color.b == 0) {
                        continue;
                    }
                    framebuffer.set_pixel(stripe, y, color);
                }
            }
        }
    }

    auto operator<<(std::ostream& stream, const SpriteRenderer& renderer) -> std::ostream& {
        stream << "SpriteRenderer{width=" << renderer.screen_width()
               << ", height=" << renderer.screen_height() << "}";
        return stream;
    }
}
