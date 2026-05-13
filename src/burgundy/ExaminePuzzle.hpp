#pragma once

#include "Puzzle.hpp"

#include <string>
#include <vector>

namespace escape::burgundy {
    class ExaminePuzzle final : public Puzzle {
    public:
        ExaminePuzzle(std::string id, std::string title, std::string prompt,
                      std::vector<std::string> revelations);

        auto kind() const -> std::string override { return "examine"; }
        void draw_ui() override;

        auto revelations() const noexcept -> const std::vector<std::string>& { return revelations_; }

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        std::vector<std::string> revelations_;
        int current_revelation_ {0};
    };
}
