#pragma once

#include <iosfwd>

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

namespace escape::app {
    class ImGuiOverlay {
    public:
        ImGuiOverlay(SDL_Window* window, SDL_Renderer* renderer);
        ~ImGuiOverlay();

        ImGuiOverlay(const ImGuiOverlay&) = delete;
        ImGuiOverlay(ImGuiOverlay&&) = delete;
        auto operator=(const ImGuiOverlay&) -> ImGuiOverlay& = delete;
        auto operator=(ImGuiOverlay&&) -> ImGuiOverlay& = delete;

        void process_event(const SDL_Event& event);

        void begin_frame();
        void end_frame(SDL_Renderer* renderer);

        auto wants_keyboard() const noexcept -> bool;
        auto wants_mouse() const noexcept -> bool;

    private:
        friend auto operator<<(std::ostream& stream, const ImGuiOverlay& overlay) -> std::ostream&;

        bool initialized_ {false};
    };

    auto operator<<(std::ostream& stream, const ImGuiOverlay& overlay) -> std::ostream&;
}
