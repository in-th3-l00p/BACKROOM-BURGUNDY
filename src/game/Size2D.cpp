#include "Size2D.hpp"

#include <ostream>

namespace escape::game {
    Size2D::Size2D(float width, float height)
        : width_(width), height_(height) {}

    auto Size2D::width() const noexcept -> float {
        return width_;
    }

    auto Size2D::height() const noexcept -> float {
        return height_;
    }

    auto Size2D::area() const noexcept -> float {
        return width_ * height_;
    }

    auto operator<<(std::ostream& stream, const Size2D& size) -> std::ostream& {
        stream << "Size2D{width=" << size.width() << ", height=" << size.height() << "}";
        return stream;
    }
} 
