#pragma once

#include "../app/Framebuffer.hpp"

#include <iosfwd>
#include <vector>

namespace escape::game {
    class Player;
    class Sprite;
    class Raycaster;

    class SpriteRenderer {
    public:
        explicit SpriteRenderer(int screen_width, int screen_height);

        void render(const Player& player,
                    const std::vector<const Sprite*>& sprites,
                    const Raycaster& raycaster,
                    app::Framebuffer& framebuffer) const;

        auto screen_width() const noexcept -> int { return screen_width_; }
        auto screen_height() const noexcept -> int { return screen_height_; }

    private:
        int screen_width_;
        int screen_height_;
    };

    auto operator<<(std::ostream& stream, const SpriteRenderer& renderer) -> std::ostream&;
}
