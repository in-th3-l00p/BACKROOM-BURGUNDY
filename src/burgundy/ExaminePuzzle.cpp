#include "ExaminePuzzle.hpp"

#include <imgui.h>

#include <ostream>
#include <utility>

namespace escape::burgundy {
    ExaminePuzzle::ExaminePuzzle(std::string id, std::string title, std::string prompt,
                                 std::vector<std::string> revelations)
        : Puzzle(std::move(id), std::move(title), std::move(prompt)),
          revelations_(std::move(revelations)) {}

    void ExaminePuzzle::draw_ui() {
        ImGui::TextWrapped("%s", prompt().c_str());
        ImGui::Separator();

        if (revelations_.empty()) {
            ImGui::TextDisabled("Nothing to reveal.");
            mark_solved();
            return;
        }

        const int last_index = static_cast<int>(revelations_.size()) - 1;
        current_revelation_ = std::min(current_revelation_, last_index);

        ImGui::TextWrapped("%s", revelations_[static_cast<std::size_t>(current_revelation_)].c_str());

        if (current_revelation_ > 0 && ImGui::Button("Look back")) {
            --current_revelation_;
        }
        if (current_revelation_ > 0) {
            ImGui::SameLine();
        }
        if (current_revelation_ < last_index) {
            if (ImGui::Button("Look closer")) {
                ++current_revelation_;
                if (current_revelation_ == last_index) {
                    mark_solved();
                }
            }
        } else {
            ImGui::TextColored(ImVec4 {0.9F, 0.8F, 0.55F, 1.0F}, "You've seen everything here.");
            mark_solved();
        }
    }

    void ExaminePuzzle::do_print(std::ostream& stream) const {
        stream << "ExaminePuzzle{id=" << id() << ", revelations=" << revelations_.size() << "}";
    }
}
