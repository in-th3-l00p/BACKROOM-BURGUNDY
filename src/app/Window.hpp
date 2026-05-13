#pragma once

#include <iosfwd>
#include <memory>
#include <string>

struct SDL_Renderer;
struct SDL_Window;

namespace escape::app {
    struct WindowConfig {
        std::string title {"escape"};
        int width {1280};
        int height {720};
    };

    struct Color {
        unsigned char r {0};
        unsigned char g {0};
        unsigned char b {0};
        unsigned char a {255};
    };

    struct Rectangle {
        float x {0.0F};
        float y {0.0F};
        float width {0.0F};
        float height {0.0F};
    };

    class Window {
    public:
        explicit Window(WindowConfig config = {});
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        auto operator=(const Window&) -> Window& = delete;
        auto operator=(Window&&) -> Window& = delete;

        void process_events();
        void clear(Color color);
        void draw_filled_rect(Rectangle rectangle, Color color);
        void present();

        auto is_open() const noexcept -> bool {
            return is_open_;
        }

        auto width() const noexcept -> int {
            return config_.width;
        }

        auto height() const noexcept -> int {
            return config_.height;
        }

    private:
        struct WindowDeleter {
            void operator()(SDL_Window* window) const noexcept;
        };

        struct RendererDeleter {
            void operator()(SDL_Renderer* renderer) const noexcept;
        };

        WindowConfig config_ {};
        std::unique_ptr<SDL_Window, WindowDeleter> window_ {nullptr};
        std::unique_ptr<SDL_Renderer, RendererDeleter> renderer_ {nullptr};
        bool is_open_ {true};
        bool sdl_initialized_ {false};
    };

    auto operator<<(std::ostream& stream, const WindowConfig& config) -> std::ostream&;
    auto operator<<(std::ostream& stream, const Color& color) -> std::ostream&;
    auto operator<<(std::ostream& stream, const Rectangle& rectangle) -> std::ostream&;
    auto operator<<(std::ostream& stream, const Window& window) -> std::ostream&;
}
