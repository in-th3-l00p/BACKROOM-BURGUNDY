#include "Tile.hpp"

#include <ostream>

namespace escape::game {
    void Tile::print(std::ostream& stream) const {
        do_print(stream);
    }

    auto Tile::shade_for_side(app::Color color, int side) -> app::Color {
        if (side == 1) {
            color.r = static_cast<unsigned char>(color.r / 2);
            color.g = static_cast<unsigned char>(color.g / 2);
            color.b = static_cast<unsigned char>(color.b / 2);
        }
        return color;
    }

    auto operator<<(std::ostream& stream, const Tile& tile) -> std::ostream& {
        tile.print(stream);
        return stream;
    }
}
