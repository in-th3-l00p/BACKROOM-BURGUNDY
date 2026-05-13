#pragma once

#include "Interactable.hpp"

#include <string>

namespace escape::burgundy {
    class PickupInteractable final : public Interactable {
    public:
        PickupInteractable(std::string label, std::string narrative, std::string item_id);

        auto kind() const -> std::string override { return "pickup"; }
        void interact(GameDirector& director) override;
        auto clone() const -> std::unique_ptr<Interactable> override;

        auto item_id() const noexcept -> const std::string& { return item_id_; }

    protected:
        void do_print(std::ostream& stream) const override;

    private:
        std::string item_id_;
    };
}
