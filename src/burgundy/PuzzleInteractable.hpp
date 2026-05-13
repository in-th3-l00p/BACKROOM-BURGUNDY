#pragma once

#include "Interactable.hpp"

#include <string>

namespace escape::burgundy {
    class PuzzleInteractable final : public Interactable {
    public:
        PuzzleInteractable(std::string label, std::string narrative, std::string puzzle_id);

        auto kind() const -> std::string override { return "puzzle"; }
        void interact(GameDirector& director) override;
        auto clone() const -> std::unique_ptr<Interactable> override;

        auto puzzle_id() const noexcept -> const std::string& { return puzzle_id_; }

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        std::string puzzle_id_;
    };
}
