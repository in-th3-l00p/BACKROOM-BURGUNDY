#include "Vector2.hpp"

#include <ostream>

namespace escape::game {
    Vector2::Vector2(float x, float y)
        : x_(x), y_(y) {}

    auto Vector2::x() const noexcept -> float {
        return x_;
    }

    auto Vector2::y() const noexcept -> float {
        return y_;
    }

    void Vector2::set_x(float x) noexcept {
        x_ = x;
    }

    void Vector2::set_y(float y) noexcept {
        y_ = y;
    }

    auto Vector2::translated(float delta_x, float delta_y) const -> Vector2 {
        return Vector2 {x_ + delta_x, y_ + delta_y};
    }

    auto Vector2::scaled(float factor) const -> Vector2 {
        return Vector2 {x_ * factor, y_ * factor};
    }

    auto Vector2::length_squared() const noexcept -> float {
        return x_ * x_ + y_ * y_;
    }

    auto operator<<(std::ostream& stream, const Vector2& vector) -> std::ostream& {
        stream << "Vector2{x=" << vector.x() << ", y=" << vector.y() << "}";
        return stream;
    }
}
