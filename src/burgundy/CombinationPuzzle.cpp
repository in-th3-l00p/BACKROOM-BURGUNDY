#include "CombinationPuzzle.hpp"

#include <imgui.h>

#include <ostream>
#include <utility>

namespace escape::burgundy {
    CombinationPuzzle::CombinationPuzzle(std::string id, std::string title, std::string prompt,
                                         std::vector<int> solution, int wheel_max)
        : Puzzle(std::move(id), std::move(title), std::move(prompt)),
          solution_(std::move(solution)),
          current_(solution_.size(), 0),
          wheel_max_(wheel_max) {}

    void CombinationPuzzle::draw_ui() {
        ImGui::TextWrapped("%s", prompt().c_str());
        ImGui::Separator();

        if (is_solved()) {
            ImGui::TextColored(ImVec4 {0.7F, 1.0F, 0.6F, 1.0F}, "The tumblers fall into place.");
            return;
        }

        for (std::size_t i = 0; i < current_.size(); ++i) {
            const auto label = "wheel " + std::to_string(i + 1);
            ImGui::PushID(static_cast<int>(i));
            ImGui::SliderInt(label.c_str(), &current_[i], 0, wheel_max_);
            ImGui::PopID();
        }

        if (ImGui::Button("Dial")) {
            if (current_ == solution_) {
                mark_solved();
                status_ = "The bolt slides back with a heavy thunk.";
            } else {
                status_ = "Nothing. The dial spins free.";
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            std::fill(current_.begin(), current_.end(), 0);
            status_.clear();
        }

        if (!status_.empty()) {
            ImGui::TextWrapped("%s", status_.c_str());
        }
    }

    void CombinationPuzzle::do_print(std::ostream& stream) const {
        stream << "CombinationPuzzle{id=" << id() << ", wheels=" << solution_.size()
               << ", solved=" << (is_solved() ? "true" : "false") << "}";
    }
}
