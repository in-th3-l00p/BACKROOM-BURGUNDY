#pragma once

#include "../app/Window.hpp"

#include <iosfwd>
#include <memory>
#include <string>

namespace escape::patterns {
    class WallShadingStrategy {
    public:
        WallShadingStrategy() = default;
        WallShadingStrategy(const WallShadingStrategy&) = default;
        WallShadingStrategy(WallShadingStrategy&&) noexcept = default;
        auto operator=(const WallShadingStrategy&) -> WallShadingStrategy& = default;
        auto operator=(WallShadingStrategy&&) noexcept -> WallShadingStrategy& = default;
        virtual ~WallShadingStrategy() = default;

        virtual auto apply(app::Color color, float distance) const -> app::Color = 0;
        virtual auto name() const -> std::string = 0;
        virtual auto clone() const -> std::unique_ptr<WallShadingStrategy> = 0;
    };

    class NoShading final : public WallShadingStrategy {
    public:
        auto apply(app::Color color, float distance) const -> app::Color override;
        auto name() const -> std::string override { return "NoShading"; }
        auto clone() const -> std::unique_ptr<WallShadingStrategy> override;
    };

    class LinearDistanceShading final : public WallShadingStrategy {
    public:
        explicit LinearDistanceShading(float max_distance = 16.0F);
        auto apply(app::Color color, float distance) const -> app::Color override;
        auto name() const -> std::string override { return "LinearDistanceShading"; }
        auto clone() const -> std::unique_ptr<WallShadingStrategy> override;

    private:
        float max_distance_;
    };

    class FogShading final : public WallShadingStrategy {
    public:
        FogShading(app::Color fog_color, float fog_density);
        auto apply(app::Color color, float distance) const -> app::Color override;
        auto name() const -> std::string override { return "FogShading"; }
        auto clone() const -> std::unique_ptr<WallShadingStrategy> override;

    private:
        app::Color fog_color_;
        float fog_density_;
    };

    auto operator<<(std::ostream& stream, const WallShadingStrategy& strategy) -> std::ostream&;
}
