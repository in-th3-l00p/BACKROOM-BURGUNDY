#include "ImGuiOverlay.hpp"

#include "../errors/Exceptions.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <ostream>

namespace escape::app {
    ImGuiOverlay::ImGuiOverlay(SDL_Window* window, SDL_Renderer* renderer) {
        IMGUI_CHECKVERSION();
        if (ImGui::CreateContext() == nullptr) {
            throw errors::InvalidConfigError {"Failed to create ImGui context"};
        }

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        auto& style = ImGui::GetStyle();
        style.WindowRounding = 6.0F;
        style.FrameRounding = 4.0F;
        style.ScrollbarRounding = 4.0F;
        style.GrabRounding = 4.0F;
        style.Colors[ImGuiCol_WindowBg] = ImVec4 {0.13F, 0.06F, 0.08F, 0.92F};
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4 {0.45F, 0.12F, 0.16F, 1.0F};
        style.Colors[ImGuiCol_TitleBg] = ImVec4 {0.20F, 0.07F, 0.09F, 1.0F};
        style.Colors[ImGuiCol_Header] = ImVec4 {0.45F, 0.12F, 0.16F, 0.6F};
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4 {0.65F, 0.18F, 0.22F, 0.85F};
        style.Colors[ImGuiCol_HeaderActive] = ImVec4 {0.75F, 0.22F, 0.28F, 1.0F};
        style.Colors[ImGuiCol_Button] = ImVec4 {0.45F, 0.12F, 0.16F, 0.85F};
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4 {0.65F, 0.18F, 0.22F, 1.0F};
        style.Colors[ImGuiCol_ButtonActive] = ImVec4 {0.85F, 0.30F, 0.30F, 1.0F};
        style.Colors[ImGuiCol_Text] = ImVec4 {0.95F, 0.85F, 0.55F, 1.0F};
        style.Colors[ImGuiCol_FrameBg] = ImVec4 {0.18F, 0.08F, 0.09F, 0.9F};

        if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
            ImGui::DestroyContext();
            throw errors::InvalidConfigError {"Failed to init ImGui SDL3 backend"};
        }
        if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            throw errors::InvalidConfigError {"Failed to init ImGui SDLRenderer3 backend"};
        }
        initialized_ = true;
    }

    ImGuiOverlay::~ImGuiOverlay() {
        if (!initialized_) {
            return;
        }
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiOverlay::process_event(const SDL_Event& event) {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    void ImGuiOverlay::begin_frame() {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiOverlay::end_frame(SDL_Renderer* renderer) {
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }

    auto ImGuiOverlay::wants_keyboard() const noexcept -> bool {
        return ImGui::GetIO().WantCaptureKeyboard;
    }

    auto ImGuiOverlay::wants_mouse() const noexcept -> bool {
        return ImGui::GetIO().WantCaptureMouse;
    }

    auto operator<<(std::ostream& stream, const ImGuiOverlay& overlay) -> std::ostream& {
        stream << "ImGuiOverlay{initialized=" << (overlay.initialized_ ? "true" : "false") << "}";
        return stream;
    }
}
