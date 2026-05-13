#include "PuzzleInteractable.hpp"

#include "GameDirector.hpp"

#include <ostream>
#include <utility>

namespace escape::burgundy {
    PuzzleInteractable::PuzzleInteractable(std::string label, std::string narrative, std::string puzzle_id)
        : Interactable(std::move(label), std::move(narrative)),
          puzzle_id_(std::move(puzzle_id)) {}

    void PuzzleInteractable::interact(GameDirector& director) {
        auto* puzzle = director.puzzle_by_id(puzzle_id_);
        if (puzzle == nullptr) {
            director.notify("Nothing happens.");
            return;
        }
        if (!narrative().empty()) {
            director.notify(narrative());
        }
        director.open_puzzle(puzzle);
    }

    auto PuzzleInteractable::clone() const -> std::unique_ptr<Interactable> {
        return std::make_unique<PuzzleInteractable>(*this);
    }

    void PuzzleInteractable::do_print(std::ostream& stream) const {
        stream << "PuzzleInteractable{label=" << label() << ", puzzle=" << puzzle_id_ << "}";
    }
}
