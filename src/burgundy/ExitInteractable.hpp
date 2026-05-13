#pragma once

#include "Interactable.hpp"

#include <string>

namespace escape::burgundy {
    class ExitInteractable final : public Interactable {
    public:
        ExitInteractable(std::string label, std::string narrative, std::string required_puzzle_id);

        auto kind() const -> std::string override { return "exit"; }
        void interact(GameDirector& director) override;
        auto clone() const -> std::unique_ptr<Interactable> override;

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        std::string required_puzzle_id_;
    };
}
