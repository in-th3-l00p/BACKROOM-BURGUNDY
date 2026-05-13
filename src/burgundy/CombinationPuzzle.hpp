#pragma once

#include "Puzzle.hpp"

#include <string>
#include <vector>

namespace escape::burgundy {
    class CombinationPuzzle final : public Puzzle {
    public:
        CombinationPuzzle(std::string id, std::string title, std::string prompt,
                          std::vector<int> solution, int wheel_max = 39);

        auto kind() const -> std::string override { return "combination"; }
        void draw_ui() override;

        auto solution() const noexcept -> const std::vector<int>& { return solution_; }
        auto current() const noexcept -> const std::vector<int>& { return current_; }

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        std::vector<int> solution_;
        std::vector<int> current_;
        int wheel_max_;
        std::string status_;
    };
}
