#include "Window.hpp"

#include <ostream>
#include <stdexcept>
#include <utility>

#include <SDL3/SDL.h>

namespace escape::app {
    auto make_window_error(const char* message) -> std::runtime_error {
        return std::runtime_error {std::string {message} + ": " + SDL_GetError()};
    }

    void ensure(bool success, const char* message) {
        if (!success) {
            throw make_window_error(message);
        }
    }

    Window::Window(WindowConfig config)
        : config_(std::move(config)) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw make_window_error("Failed to initialize SDL video");
        }

        sdl_initialized_ = true;
        window_.reset(SDL_CreateWindow(config_.title.c_str(), config_.width, config_.height, SDL_WINDOW_RESIZABLE));

        if (window_ == nullptr) {
            SDL_Quit();
            sdl_initialized_ = false;
            throw make_window_error("Failed to create SDL window");
        }

        renderer_.reset(SDL_CreateRenderer(window_.get(), nullptr));

        if (renderer_ == nullptr) {
            window_.reset();
            SDL_Quit();
            sdl_initialized_ = false;
            throw make_window_error("Failed to create SDL renderer");
        }
    }

    Window::~Window() {
        renderer_.reset();
        window_.reset();

        if (sdl_initialized_) {
            SDL_Quit();
        }
    }

    void Window::process_events() {
        SDL_Event event {};

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                is_open_ = false;
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID == SDL_GetWindowID(window_.get())) {
                    is_open_ = false;
                }
                break;
            default:
                break;
            }
        }
    }

    void Window::clear(Color color) {
        ensure(SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a),
            "Failed to set renderer clear color");
        ensure(SDL_RenderClear(renderer_.get()), "Failed to clear renderer");
    }

    void Window::draw_filled_rect(Rectangle rectangle, Color color) {
        const auto sdl_rectangle = SDL_FRect {rectangle.x, rectangle.y, rectangle.width, rectangle.height};

        ensure(SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a),
            "Failed to set renderer draw color");
        ensure(SDL_RenderFillRect(renderer_.get(), &sdl_rectangle), "Failed to draw filled rectangle");
    }

    void Window::present() {
        ensure(SDL_RenderPresent(renderer_.get()), "Failed to present renderer");
    }

    void Window::WindowDeleter::operator()(SDL_Window* window) const noexcept {
        if (window != nullptr) {
            SDL_DestroyWindow(window);
        }
    }

    void Window::RendererDeleter::operator()(SDL_Renderer* renderer) const noexcept {
        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
        }
    }

    auto operator<<(std::ostream& stream, const WindowConfig& config) -> std::ostream& {
        stream << "WindowConfig{title=" << config.title
            << ", width=" << config.width
            << ", height=" << config.height << "}";
        return stream;
    }

    auto operator<<(std::ostream& stream, const Color& color) -> std::ostream& {
        stream << "Color{r=" << static_cast<int>(color.r)
            << ", g=" << static_cast<int>(color.g)
            << ", b=" << static_cast<int>(color.b)
            << ", a=" << static_cast<int>(color.a) << "}";
        return stream;
    }

    auto operator<<(std::ostream& stream, const Rectangle& rectangle) -> std::ostream& {
        stream << "Rectangle{x=" << rectangle.x
            << ", y=" << rectangle.y
            << ", width=" << rectangle.width
            << ", height=" << rectangle.height << "}";
        return stream;
    }

    auto operator<<(std::ostream& stream, const Window& window) -> std::ostream& {
        stream << "Window{is_open=" << (window.is_open() ? "true" : "false")
            << ", width=" << window.width()
            << ", height=" << window.height() << "}";
        return stream;
    }
}
