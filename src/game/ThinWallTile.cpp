#include "ThinWallTile.hpp"

#include "../errors/Exceptions.hpp"

#include <ostream>

namespace escape::game {
    ThinWallTile::ThinWallTile(int cell_id, std::shared_ptr<Texture> texture)
        : cell_id_(cell_id), texture_(std::move(texture)) {
        if (texture_ == nullptr) {
            throw errors::InvalidConfigError {"ThinWallTile requires a non-null texture"};
        }
    }

    auto ThinWallTile::clone() const -> std::unique_ptr<Tile> {
        return std::make_unique<ThinWallTile>(*this);
    }

    auto ThinWallTile::base_color() const -> app::Color {
        return texture_->sample(0, 0);
    }

    auto ThinWallTile::sample(float wall_u, float wall_v, int side) const -> app::Color {
        return shade_for_side(texture_->sample_normalized(wall_u, wall_v), side);
    }

    void ThinWallTile::do_print(std::ostream& stream) const {
        stream << "ThinWallTile{id=" << cell_id_ << "}";
    }
}
