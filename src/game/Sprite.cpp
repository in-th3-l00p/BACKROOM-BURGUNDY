#include "Sprite.hpp"

#include "../errors/Exceptions.hpp"

#include <ostream>
#include <utility>

namespace escape::game {
    Sprite::Sprite(std::string name, Vector2 position, std::shared_ptr<Texture> texture, float scale)
        : name_(std::move(name)),
          position_(position),
          texture_(std::move(texture)),
          scale_(scale) {
        if (texture_ == nullptr) {
            throw errors::InvalidConfigError {"Sprite requires a non-null texture"};
        }
        if (scale_ <= 0.0F) {
            throw errors::InvalidConfigError {"Sprite scale must be positive"};
        }
    }

    void Sprite::set_position(Vector2 position) noexcept {
        position_ = position;
    }

    auto operator<<(std::ostream& stream, const Sprite& sprite) -> std::ostream& {
        stream << "Sprite{name=" << sprite.name()
               << ", position=" << sprite.position()
               << ", scale=" << sprite.scale()
               << ", texture=" << sprite.texture() << "}";
        return stream;
    }
}
