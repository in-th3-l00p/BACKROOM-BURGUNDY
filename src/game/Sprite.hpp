#pragma once

#include "Texture.hpp"
#include "Vector2.hpp"

#include <iosfwd>
#include <memory>
#include <string>

namespace escape::game {
    class Sprite {
    public:
        Sprite(std::string name, Vector2 position, std::shared_ptr<Texture> texture, float scale = 1.0F);

        auto name() const noexcept -> const std::string& { return name_; }
        auto position() const noexcept -> const Vector2& { return position_; }
        auto texture() const noexcept -> const Texture& { return *texture_; }
        auto scale() const noexcept -> float { return scale_; }

        void set_position(Vector2 position) noexcept;

    private:
        std::string name_;
        Vector2 position_;
        std::shared_ptr<Texture> texture_;
        float scale_;
    };

    auto operator<<(std::ostream& stream, const Sprite& sprite) -> std::ostream&;
}
