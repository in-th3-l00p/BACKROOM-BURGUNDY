#include "PickupInteractable.hpp"

#include "GameDirector.hpp"

#include <ostream>
#include <utility>

namespace escape::burgundy {
    PickupInteractable::PickupInteractable(std::string label, std::string narrative, std::string item_id)
        : Interactable(std::move(label), std::move(narrative)),
          item_id_(std::move(item_id)) {}

    void PickupInteractable::interact(GameDirector& director) {
        if (is_consumed()) {
            return;
        }
        director.grant_item_by_id(item_id_);
        if (!narrative().empty()) {
            director.notify(narrative());
        }
        consume();
    }

    auto PickupInteractable::clone() const -> std::unique_ptr<Interactable> {
        return std::make_unique<PickupInteractable>(*this);
    }

    void PickupInteractable::do_print(std::ostream& stream) const {
        stream << "PickupInteractable{label=" << label() << ", item=" << item_id_ << "}";
    }
}
