#pragma once

#include <iosfwd>

namespace escape::game {
    class Vector2 {
    public:
        Vector2(float x, float y);

        auto x() const noexcept -> float;
        auto y() const noexcept -> float;
        void set_x(float x) noexcept;
        void set_y(float y) noexcept;

        auto translated(float delta_x, float delta_y) const -> Vector2;
        auto scaled(float factor) const -> Vector2;
        auto length_squared() const noexcept -> float;

    private:
        float x_ {0.0F};
        float y_ {0.0F};
    };

    auto operator<<(std::ostream& stream, const Vector2& vector) -> std::ostream&;
}
