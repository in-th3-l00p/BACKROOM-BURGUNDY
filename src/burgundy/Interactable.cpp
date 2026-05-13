#include "Interactable.hpp"

#include <ostream>
#include <utility>

namespace escape::burgundy {
    Interactable::Interactable(std::string label, std::string narrative)
        : label_(std::move(label)), narrative_(std::move(narrative)) {}

    void Interactable::print(std::ostream& stream) const {
        do_print(stream);
    }

    auto operator<<(std::ostream& stream, const Interactable& interactable) -> std::ostream& {
        interactable.print(stream);
        return stream;
    }
}
