#include "Puzzle.hpp"

#include <ostream>
#include <utility>

namespace escape::burgundy {
    Puzzle::Puzzle(std::string id, std::string title, std::string prompt)
        : id_(std::move(id)), title_(std::move(title)), prompt_(std::move(prompt)) {}

    void Puzzle::set_reward_item_id(std::string reward_item_id) noexcept {
        reward_item_id_ = std::move(reward_item_id);
    }

    void Puzzle::set_reward_message(std::string reward_message) noexcept {
        reward_message_ = std::move(reward_message);
    }

    void Puzzle::add_required_item(std::string item_id) {
        required_item_ids_.push_back(std::move(item_id));
    }

    void Puzzle::print(std::ostream& stream) const {
        do_print(stream);
    }

    auto operator<<(std::ostream& stream, const Puzzle& puzzle) -> std::ostream& {
        puzzle.print(stream);
        return stream;
    }
}
