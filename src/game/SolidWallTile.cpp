#include "SolidWallTile.hpp"

#include <ostream>

namespace escape::game {
    SolidWallTile::SolidWallTile(int cell_id, app::Color color)
        : cell_id_(cell_id), color_(color) {}

    auto SolidWallTile::clone() const -> std::unique_ptr<Tile> {
        return std::make_unique<SolidWallTile>(*this);
    }

    auto SolidWallTile::sample(float, float, int side) const -> app::Color {
        return shade_for_side(color_, side);
    }

    void SolidWallTile::do_print(std::ostream& stream) const {
        stream << "SolidWallTile{id=" << cell_id_ << ", color=" << color_ << "}";
    }
}
