#pragma once

#include "Puzzle.hpp"

#include <array>
#include <string>

namespace escape::burgundy {
    class KeypadPuzzle final : public Puzzle {
    public:
        KeypadPuzzle(std::string id, std::string title, std::string prompt, std::string solution);

        auto kind() const -> std::string override { return "keypad"; }
        void draw_ui() override;

        auto solution() const noexcept -> const std::string& { return solution_; }
        auto entry() const noexcept -> const std::string& { return entry_; }

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        std::string solution_;
        std::string entry_;
        std::array<char, 64> buffer_ {};
        std::string status_;
    };
}
