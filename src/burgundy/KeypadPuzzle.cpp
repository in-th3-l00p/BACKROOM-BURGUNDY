#include "KeypadPuzzle.hpp"

#include <imgui.h>

#include <cstring>
#include <ostream>
#include <utility>

namespace escape::burgundy {
    KeypadPuzzle::KeypadPuzzle(std::string id, std::string title, std::string prompt, std::string solution)
        : Puzzle(std::move(id), std::move(title), std::move(prompt)),
          solution_(std::move(solution)) {}

    void KeypadPuzzle::draw_ui() {
        ImGui::TextWrapped("%s", prompt().c_str());
        ImGui::Separator();

        if (is_solved()) {
            ImGui::TextColored(ImVec4 {0.7F, 1.0F, 0.6F, 1.0F}, "ACCEPTED");
            return;
        }

        ImGui::InputText("##entry", buffer_.data(), buffer_.size(),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);

        if (ImGui::Button("Submit")) {
            entry_ = buffer_.data();
            if (entry_ == solution_) {
                mark_solved();
                status_ = "The latch clicks open.";
            } else {
                status_ = "A muted buzz. Try again.";
                std::memset(buffer_.data(), 0, buffer_.size());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            std::memset(buffer_.data(), 0, buffer_.size());
            status_.clear();
        }

        if (!status_.empty()) {
            ImGui::TextWrapped("%s", status_.c_str());
        }
    }

    void KeypadPuzzle::do_print(std::ostream& stream) const {
        stream << "KeypadPuzzle{id=" << id()
               << ", solved=" << (is_solved() ? "true" : "false") << "}";
    }
}
