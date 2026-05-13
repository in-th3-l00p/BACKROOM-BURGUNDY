#include "EmptyTile.hpp"

#include <ostream>

namespace escape::game {
    EmptyTile::EmptyTile()
        : floor_tint_(app::Color {.r = 40, .g = 40, .b = 40, .a = 255}) {}

    EmptyTile::EmptyTile(app::Color floor_tint)
        : floor_tint_(floor_tint) {}

    auto EmptyTile::clone() const -> std::unique_ptr<Tile> {
        return std::make_unique<EmptyTile>(*this);
    }

    auto EmptyTile::sample(float, float, int) const -> app::Color {
        return floor_tint_;
    }

    void EmptyTile::do_print(std::ostream& stream) const {
        stream << "EmptyTile{tint=" << floor_tint_ << "}";
    }
}
