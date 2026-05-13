#pragma once

#include <iosfwd>

namespace escape::game {
    class Size2D {
    public:
        Size2D(float width, float height);

        auto width() const noexcept -> float;
        auto height() const noexcept -> float;
        auto area() const noexcept -> float;

    private:
        float width_ {0.0F};
        float height_ {0.0F};
    };

    auto operator<<(std::ostream& stream, const Size2D& size) -> std::ostream&;
}
