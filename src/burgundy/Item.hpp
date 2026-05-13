#pragma once

#include <iosfwd>
#include <string>

namespace escape::burgundy {
    class Item {
    public:
        Item(std::string id, std::string title, std::string description);

        auto id() const noexcept -> const std::string& { return id_; }
        auto title() const noexcept -> const std::string& { return title_; }
        auto description() const noexcept -> const std::string& { return description_; }

    private:
        std::string id_;
        std::string title_;
        std::string description_;
    };

    auto operator<<(std::ostream& stream, const Item& item) -> std::ostream&;
}
