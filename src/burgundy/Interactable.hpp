#pragma once

#include <iosfwd>
#include <memory>
#include <string>

namespace escape::burgundy {
    class GameDirector;

    class Interactable {
    public:
        Interactable(std::string label, std::string narrative);
        Interactable(const Interactable&) = default;
        Interactable(Interactable&&) noexcept = default;
        auto operator=(const Interactable&) -> Interactable& = default;
        auto operator=(Interactable&&) noexcept -> Interactable& = default;
        virtual ~Interactable() = default;

        auto label() const noexcept -> const std::string& { return label_; }
        auto narrative() const noexcept -> const std::string& { return narrative_; }
        auto is_consumed() const noexcept -> bool { return consumed_; }

        virtual auto kind() const -> std::string = 0;
        virtual void interact(GameDirector& director) = 0;
        virtual auto clone() const -> std::unique_ptr<Interactable> = 0;

        void print(std::ostream& stream) const;

    protected:
        virtual void do_print(std::ostream& stream) const = 0;

        void consume() noexcept { consumed_ = true; }

    private:
        std::string label_;
        std::string narrative_;
        bool consumed_ {false};
    };

    auto operator<<(std::ostream& stream, const Interactable& interactable) -> std::ostream&;
}
