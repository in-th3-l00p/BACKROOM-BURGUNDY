#include "EmissiveWallTile.hpp"

#include <algorithm>
#include <cmath>
#include <ostream>

namespace escape::game {
    EmissiveWallTile::EmissiveWallTile(int cell_id, app::Color glow_color, float pulse_strength)
        : cell_id_(cell_id), glow_color_(glow_color), pulse_strength_(pulse_strength) {}

    auto EmissiveWallTile::clone() const -> std::unique_ptr<Tile> {
        return std::make_unique<EmissiveWallTile>(*this);
    }

    auto EmissiveWallTile::sample(float, float wall_v, int) const -> app::Color {
        const auto pulse = pulse_strength_ * std::sin(wall_v * 6.2831853F * 2.0F);
        const auto boost = [&](unsigned char channel) {
            const auto value = static_cast<float>(channel) * (1.0F + pulse);
            return static_cast<unsigned char>(std::clamp(value, 0.0F, 255.0F));
        };
        return app::Color {
            .r = boost(glow_color_.r),
            .g = boost(glow_color_.g),
            .b = boost(glow_color_.b),
            .a = glow_color_.a,
        };
    }

    void EmissiveWallTile::do_print(std::ostream& stream) const {
        stream << "EmissiveWallTile{id=" << cell_id_
               << ", glow=" << glow_color_
               << ", pulse=" << pulse_strength_ << "}";
    }
}
