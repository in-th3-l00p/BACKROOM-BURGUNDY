#include "ExitInteractable.hpp"

#include "GameDirector.hpp"

#include <ostream>
#include <utility>

namespace escape::burgundy {
    ExitInteractable::ExitInteractable(std::string label, std::string narrative, std::string required_puzzle_id)
        : Interactable(std::move(label), std::move(narrative)),
          required_puzzle_id_(std::move(required_puzzle_id)) {}

    void ExitInteractable::interact(GameDirector& director) {
        if (!required_puzzle_id_.empty()) {
            const auto* required = director.puzzle_by_id(required_puzzle_id_);
            if (required == nullptr || !required->is_solved()) {
                director.notify("Locked. The safe still holds the key.");
                return;
            }
        }
        if (!narrative().empty()) {
            director.notify(narrative());
        }
        director.declare_won();
    }

    auto ExitInteractable::clone() const -> std::unique_ptr<Interactable> {
        return std::make_unique<ExitInteractable>(*this);
    }

    void ExitInteractable::do_print(std::ostream& stream) const {
        stream << "ExitInteractable{label=" << label()
               << ", requires=" << required_puzzle_id_ << "}";
    }
}
