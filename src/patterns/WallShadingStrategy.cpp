#include "WallShadingStrategy.hpp"

#include <algorithm>
#include <cmath>
#include <ostream>

namespace escape::patterns {
    auto NoShading::apply(app::Color color, float) const -> app::Color {
        return color;
    }

    auto NoShading::clone() const -> std::unique_ptr<WallShadingStrategy> {
        return std::make_unique<NoShading>(*this);
    }

    LinearDistanceShading::LinearDistanceShading(float max_distance)
        : max_distance_(max_distance > 0.0F ? max_distance : 1.0F) {}

    auto LinearDistanceShading::apply(app::Color color, float distance) const -> app::Color {
        const float t = std::clamp(distance / max_distance_, 0.0F, 1.0F);
        const float factor = 1.0F - t;
        return app::Color {
            .r = static_cast<unsigned char>(static_cast<float>(color.r) * factor),
            .g = static_cast<unsigned char>(static_cast<float>(color.g) * factor),
            .b = static_cast<unsigned char>(static_cast<float>(color.b) * factor),
            .a = color.a,
        };
    }

    auto LinearDistanceShading::clone() const -> std::unique_ptr<WallShadingStrategy> {
        return std::make_unique<LinearDistanceShading>(*this);
    }

    FogShading::FogShading(app::Color fog_color, float fog_density)
        : fog_color_(fog_color),
          fog_density_(std::max(fog_density, 0.0F)) {}

    auto FogShading::apply(app::Color color, float distance) const -> app::Color {
        const float t = std::clamp(1.0F - std::exp(-fog_density_ * distance), 0.0F, 1.0F);
        const auto mix = [&](unsigned char from, unsigned char to) -> unsigned char {
            return static_cast<unsigned char>(
                static_cast<float>(from) * (1.0F - t) + static_cast<float>(to) * t);
        };
        return app::Color {
            .r = mix(color.r, fog_color_.r),
            .g = mix(color.g, fog_color_.g),
            .b = mix(color.b, fog_color_.b),
            .a = color.a,
        };
    }

    auto FogShading::clone() const -> std::unique_ptr<WallShadingStrategy> {
        return std::make_unique<FogShading>(*this);
    }

    auto operator<<(std::ostream& stream, const WallShadingStrategy& strategy) -> std::ostream& {
        stream << "WallShadingStrategy{name=" << strategy.name() << "}";
        return stream;
    }
}
