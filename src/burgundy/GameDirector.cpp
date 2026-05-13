#include "GameDirector.hpp"

#include <algorithm>
#include <ostream>
#include <utility>

namespace escape::burgundy {
    GameDirector::GameDirector() = default;

    void GameDirector::open_puzzle(Puzzle* puzzle) noexcept {
        active_puzzle_ = puzzle;
    }

    void GameDirector::close_puzzle() noexcept {
        active_puzzle_ = nullptr;
    }

    void GameDirector::notify(std::string message) {
        status_line_ = message;
        narrative_log_.push_back(std::move(message));
        if (narrative_log_.size() > 64) {
            narrative_log_.erase(narrative_log_.begin());
        }
    }

    void GameDirector::grant_item(std::string id, std::string title, std::string description) {
        auto item = Item {id, std::move(title), std::move(description)};
        item_templates_.insert_or_assign(id, item);
        inventory_.add(std::move(item));
        notify("Picked up: " + id);
    }

    void GameDirector::grant_item_by_id(const std::string& id) {
        const auto iterator = item_templates_.find(id);
        if (iterator == item_templates_.end()) {
            inventory_.add(Item {id, id, ""});
            notify("Picked up: " + id);
            return;
        }
        inventory_.add(iterator->second);
        notify("Picked up: " + iterator->second.title());
    }

    void GameDirector::declare_won() {
        won_ = true;
        notify("Sunrise. You're out.");
    }

    void GameDirector::register_item_template(Item item) {
        const auto id = item.id();
        item_templates_.insert_or_assign(id, std::move(item));
    }

    void GameDirector::register_puzzle(std::unique_ptr<Puzzle> puzzle) {
        if (puzzle != nullptr) {
            puzzles_.push_back(std::move(puzzle));
        }
    }

    void GameDirector::register_interactable(int x, int y, std::unique_ptr<Interactable> interactable) {
        if (interactable != nullptr) {
            interactables_[TileKey {x, y}] = std::move(interactable);
        }
    }

    auto GameDirector::puzzle_by_id(const std::string& id) -> Puzzle* {
        for (auto& puzzle : puzzles_) {
            if (puzzle->id() == id) {
                return puzzle.get();
            }
        }
        return nullptr;
    }

    auto GameDirector::interactable_at(int x, int y) -> Interactable* {
        const auto iterator = interactables_.find(TileKey {x, y});
        if (iterator == interactables_.end()) {
            return nullptr;
        }
        return iterator->second.get();
    }

    void GameDirector::consume_interactable_at(int x, int y) {
        interactables_.erase(TileKey {x, y});
    }

    auto GameDirector::item_template(const std::string& id) const -> const Item* {
        const auto iterator = item_templates_.find(id);
        if (iterator == item_templates_.end()) {
            return nullptr;
        }
        return &iterator->second;
    }

    auto GameDirector::solved_puzzle_count() const noexcept -> std::size_t {
        return static_cast<std::size_t>(std::count_if(puzzles_.begin(), puzzles_.end(),
            [](const std::unique_ptr<Puzzle>& puzzle) { return puzzle->is_solved(); }));
    }

    auto operator<<(std::ostream& stream, const GameDirector& director) -> std::ostream& {
        stream << "GameDirector{won=" << (director.is_won() ? "true" : "false")
               << ", puzzles=" << director.total_puzzle_count()
               << ", solved=" << director.solved_puzzle_count()
               << ", inventory=" << director.inventory() << "}";
        return stream;
    }
}
