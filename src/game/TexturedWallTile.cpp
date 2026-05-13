#include "TexturedWallTile.hpp"

#include "../errors/Exceptions.hpp"

#include <ostream>

namespace escape::game {
    TexturedWallTile::TexturedWallTile(int cell_id, std::shared_ptr<Texture> texture)
        : cell_id_(cell_id), texture_(std::move(texture)) {
        if (texture_ == nullptr) {
            throw errors::InvalidConfigError {"TexturedWallTile requires a non-null texture"};
        }
    }

    auto TexturedWallTile::clone() const -> std::unique_ptr<Tile> {
        return std::make_unique<TexturedWallTile>(cell_id_, texture_);
    }

    auto TexturedWallTile::base_color() const -> app::Color {
        return texture_->sample(0, 0);
    }

    auto TexturedWallTile::sample(float wall_u, float wall_v, int side) const -> app::Color {
        return shade_for_side(texture_->sample_normalized(wall_u, wall_v), side);
    }

    void TexturedWallTile::do_print(std::ostream& stream) const {
        stream << "TexturedWallTile{id=" << cell_id_ << ", texture=" << *texture_ << "}";
    }
}
