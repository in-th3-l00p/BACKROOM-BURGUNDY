#pragma once

#include "../errors/Exceptions.hpp"

#include <algorithm>
#include <ostream>
#include <utility>

namespace escape::templates {
    template <typename T>
    class Bounded {
    public:
        Bounded(T value, T minimum, T maximum)
            : minimum_(minimum), maximum_(maximum), value_(std::clamp(value, minimum, maximum)) {
            if (minimum > maximum) {
                throw errors::InvalidConfigError {"Bounded minimum must not exceed maximum"};
            }
        }

        auto value() const noexcept -> T { return value_; }
        auto minimum() const noexcept -> T { return minimum_; }
        auto maximum() const noexcept -> T { return maximum_; }

        void set(T new_value) noexcept {
            value_ = std::clamp(new_value, minimum_, maximum_);
        }

        auto add(T delta) noexcept -> T {
            set(value_ + delta);
            return value_;
        }

        auto scaled(float factor) const -> Bounded {
            return Bounded {static_cast<T>(value_ * factor), minimum_, maximum_};
        }

    private:
        T minimum_;
        T maximum_;
        T value_;
    };

    template <typename T>
    auto operator<<(std::ostream& stream, const Bounded<T>& bounded) -> std::ostream& {
        stream << "Bounded{value=" << bounded.value()
               << ", min=" << bounded.minimum()
               << ", max=" << bounded.maximum() << "}";
        return stream;
    }

    template <typename T>
    auto clamp_into(const Bounded<T>& bounded, T value) -> T {
        return std::clamp(value, bounded.minimum(), bounded.maximum());
    }
}
