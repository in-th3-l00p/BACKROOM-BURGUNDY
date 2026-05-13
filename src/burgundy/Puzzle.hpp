#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace escape::burgundy {
    class Puzzle {
    public:
        Puzzle(std::string id, std::string title, std::string prompt);
        Puzzle(const Puzzle&) = default;
        Puzzle(Puzzle&&) noexcept = default;
        auto operator=(const Puzzle&) -> Puzzle& = default;
        auto operator=(Puzzle&&) noexcept -> Puzzle& = default;
        virtual ~Puzzle() = default;

        auto id() const noexcept -> const std::string& { return id_; }
        auto title() const noexcept -> const std::string& { return title_; }
        auto prompt() const noexcept -> const std::string& { return prompt_; }

        auto reward_item_id() const noexcept -> const std::string& { return reward_item_id_; }
        void set_reward_item_id(std::string reward_item_id) noexcept;

        auto reward_message() const noexcept -> const std::string& { return reward_message_; }
        void set_reward_message(std::string reward_message) noexcept;

        auto is_solved() const noexcept -> bool { return solved_; }
        void mark_solved() noexcept { solved_ = true; }

        auto required_item_ids() const noexcept -> const std::vector<std::string>& { return required_item_ids_; }
        void add_required_item(std::string item_id);

        virtual void draw_ui() = 0;
        virtual auto kind() const -> std::string = 0;

        void print(std::ostream& stream) const;

    protected:
        virtual void do_print(std::ostream& stream) const = 0;

    private:
        std::string id_;
        std::string title_;
        std::string prompt_;
        std::string reward_item_id_;
        std::string reward_message_;
        std::vector<std::string> required_item_ids_;
        bool solved_ {false};
    };

    auto operator<<(std::ostream& stream, const Puzzle& puzzle) -> std::ostream&;
}
