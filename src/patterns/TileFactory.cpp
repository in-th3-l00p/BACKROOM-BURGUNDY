#include "TileFactory.hpp"

#include "../errors/Exceptions.hpp"
#include "../game/DoorTile.hpp"
#include "../game/EmissiveWallTile.hpp"
#include "../game/EmptyTile.hpp"
#include "../game/SolidWallTile.hpp"
#include "../game/Texture.hpp"
#include "../game/TexturedWallTile.hpp"
#include "../game/ThinWallTile.hpp"

#include <ostream>

namespace escape::patterns {
    namespace {
        auto brick_texture() -> std::shared_ptr<game::Texture> {
            static auto t = std::make_shared<game::Texture>(game::Texture::make_brick(
                64, 64,
                app::Color {.r = 170, .g = 70, .b = 60, .a = 255},
                app::Color {.r = 50,  .g = 30, .b = 20, .a = 255}));
            return t;
        }

        auto grid_texture() -> std::shared_ptr<game::Texture> {
            static auto t = std::make_shared<game::Texture>(game::Texture::make_grid(
                64, 64,
                app::Color {.r = 60,  .g = 90,  .b = 150, .a = 255},
                app::Color {.r = 200, .g = 230, .b = 255, .a = 255}));
            return t;
        }

        auto door_texture() -> std::shared_ptr<game::Texture> {
            constexpr int width = 64;
            constexpr int height = 64;
            auto pixels = std::vector<app::Color> {};
            pixels.reserve(static_cast<std::size_t>(width * height));
            const auto plank = app::Color {.r = 150, .g = 95, .b = 55, .a = 255};
            const auto deep_plank = app::Color {.r = 110, .g = 65, .b = 35, .a = 255};
            const auto frame = app::Color {.r = 60, .g = 30, .b = 15, .a = 255};
            const auto handle = app::Color {.r = 230, .g = 200, .b = 80, .a = 255};
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const bool border = x < 3 || x >= width - 3 || y < 3 || y >= height - 3;
                    const bool horizontal_band = y == height / 3 || y == 2 * height / 3
                                              || y == height / 3 + 1 || y == 2 * height / 3 + 1;
                    const bool plank_seam = (x % 16) < 1;
                    const bool handle_pixel = (x >= width - 14 && x <= width - 9)
                                            && (y >= height / 2 - 3 && y <= height / 2 + 3);
                    auto color = (y % 2 == 0) ? plank : deep_plank;
                    if (plank_seam) color = frame;
                    if (horizontal_band) color = frame;
                    if (border) color = frame;
                    if (handle_pixel) color = handle;
                    pixels.push_back(color);
                }
            }
            static auto t = std::make_shared<game::Texture>(width, height, std::move(pixels));
            return t;
        }

        auto damask_texture() -> std::shared_ptr<game::Texture> {
            constexpr int width = 64;
            constexpr int height = 64;
            auto pixels = std::vector<app::Color> {};
            pixels.reserve(static_cast<std::size_t>(width * height));
            const auto base = app::Color {.r = 92, .g = 18, .b = 28, .a = 255};
            const auto highlight = app::Color {.r = 150, .g = 110, .b = 40, .a = 255};
            const auto deep = app::Color {.r = 50, .g = 8, .b = 14, .a = 255};
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const int diamond = std::abs(((x + y) % 32) - 16) + std::abs(((x - y + 32) % 32) - 16);
                    auto color = base;
                    if (diamond < 6) color = highlight;
                    else if (diamond > 24) color = deep;
                    if ((x % 32 == 0) || (y % 32 == 0)) color = deep;
                    pixels.push_back(color);
                }
            }
            static auto t = std::make_shared<game::Texture>(width, height, std::move(pixels));
            return t;
        }

        auto velvet_brick_texture() -> std::shared_ptr<game::Texture> {
            static auto t = std::make_shared<game::Texture>(game::Texture::make_brick(
                64, 64,
                app::Color {.r = 110, .g = 22, .b = 32, .a = 255},
                app::Color {.r = 35,  .g = 8,  .b = 14, .a = 255}));
            return t;
        }

        auto brass_panel_texture() -> std::shared_ptr<game::Texture> {
            constexpr int width = 64;
            constexpr int height = 64;
            auto pixels = std::vector<app::Color> {};
            pixels.reserve(static_cast<std::size_t>(width * height));
            const auto brass = app::Color {.r = 170, .g = 130, .b = 50, .a = 255};
            const auto patina = app::Color {.r = 65, .g = 100, .b = 75, .a = 255};
            const auto seam = app::Color {.r = 40, .g = 30, .b = 15, .a = 255};
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const bool is_seam = (x % 16 == 0) || (y % 32 == 0);
                    const bool is_patina = ((x * y * 13) % 31) < 6;
                    auto color = brass;
                    if (is_patina) color = patina;
                    if (is_seam) color = seam;
                    pixels.push_back(color);
                }
            }
            static auto t = std::make_shared<game::Texture>(width, height, std::move(pixels));
            return t;
        }

        auto kitchen_tile_texture() -> std::shared_ptr<game::Texture> {
            static auto t = std::make_shared<game::Texture>(game::Texture::make_checker(
                64, 64,
                app::Color {.r = 215, .g = 210, .b = 185, .a = 255},
                app::Color {.r = 95,  .g = 88,  .b = 70,  .a = 255},
                8));
            return t;
        }

        auto bead_curtain_texture() -> std::shared_ptr<game::Texture> {
            constexpr int width = 64;
            constexpr int height = 64;
            auto pixels = std::vector<app::Color> {};
            pixels.reserve(static_cast<std::size_t>(width * height));
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const bool string = (x % 14) < 2;
                    const bool bead = string && (y % 9) < 4;
                    if (bead) {
                        pixels.push_back(app::Color {.r = 220, .g = 170, .b = 60, .a = 255});
                    } else if (string) {
                        pixels.push_back(app::Color {.r = 130, .g = 90, .b = 40, .a = 255});
                    } else {
                        pixels.push_back(app::Color {.r = 0, .g = 0, .b = 0, .a = 0});
                    }
                }
            }
            static auto t = std::make_shared<game::Texture>(width, height, std::move(pixels));
            return t;
        }

        auto vault_door_texture() -> std::shared_ptr<game::Texture> {
            constexpr int width = 64;
            constexpr int height = 64;
            auto pixels = std::vector<app::Color> {};
            pixels.reserve(static_cast<std::size_t>(width * height));
            const auto steel = app::Color {.r = 90, .g = 75, .b = 65, .a = 255};
            const auto highlight = app::Color {.r = 160, .g = 140, .b = 90, .a = 255};
            const auto rivet = app::Color {.r = 200, .g = 180, .b = 110, .a = 255};
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const int cx = x - width / 2;
                    const int cy = y - height / 2;
                    const int rr = cx * cx + cy * cy;
                    auto color = steel;
                    if (rr < 24 * 24 && rr > 18 * 18) color = highlight;
                    if (rr < 10 * 10) color = highlight;
                    for (int spoke = 0; spoke < 4; ++spoke) {
                        const float angle = static_cast<float>(spoke) * 1.5708F;
                        const int rx = static_cast<int>(20.0F * std::cos(angle));
                        const int ry = static_cast<int>(20.0F * std::sin(angle));
                        if (std::abs(cx - rx) + std::abs(cy - ry) < 3) color = rivet;
                    }
                    pixels.push_back(color);
                }
            }
            static auto t = std::make_shared<game::Texture>(width, height, std::move(pixels));
            return t;
        }

        auto fence_texture() -> std::shared_ptr<game::Texture> {
            constexpr int width = 64;
            constexpr int height = 64;
            auto pixels = std::vector<app::Color> {};
            pixels.reserve(static_cast<std::size_t>(width * height));
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const bool bar = (x % 16) < 4;
                    pixels.push_back(bar
                        ? app::Color {.r = 200, .g = 200, .b = 200, .a = 255}
                        : app::Color {.r = 0,   .g = 0,   .b = 0,   .a = 0});
                }
            }
            static auto t = std::make_shared<game::Texture>(width, height, std::move(pixels));
            return t;
        }
    }

    TileFactory::TileFactory() = default;

    void TileFactory::register_builder(int cell_id, Builder builder) {
        if (builder == nullptr) {
            throw errors::InvalidConfigError {"TileFactory builder must not be null"};
        }
        builders_[cell_id] = std::move(builder);
    }

    auto TileFactory::create(int cell_id) const -> std::unique_ptr<game::Tile> {
        const auto iterator = builders_.find(cell_id);
        if (iterator == builders_.end()) {
            throw errors::InvalidConfigError {"TileFactory has no builder for the requested cell id"};
        }
        return iterator->second();
    }

    auto TileFactory::with_demo_registry() -> TileFactory {
        auto factory = TileFactory {};
        factory.register_builder(0, [] { return std::make_unique<game::EmptyTile>(); });
        factory.register_builder(1, [] { return std::make_unique<game::TexturedWallTile>(1, brick_texture()); });
        factory.register_builder(2, [] {
            return std::make_unique<game::SolidWallTile>(2, app::Color {.r = 50, .g = 200, .b = 90, .a = 255});
        });
        factory.register_builder(3, [] { return std::make_unique<game::TexturedWallTile>(3, grid_texture()); });
        factory.register_builder(4, [] { return std::make_unique<game::TexturedWallTile>(4,
            std::make_shared<game::Texture>(game::Texture::make_checker(64, 64,
                app::Color {.r = 210, .g = 210, .b = 210, .a = 255},
                app::Color {.r = 70,  .g = 70,  .b = 70,  .a = 255},
                8))); });
        factory.register_builder(5, [] {
            return std::make_unique<game::EmissiveWallTile>(5,
                app::Color {.r = 240, .g = 200, .b = 70, .a = 255}, 0.35F);
        });
        factory.register_builder(6, [] { return std::make_unique<game::DoorTile>(6, door_texture()); });
        factory.register_builder(7, [] { return std::make_unique<game::ThinWallTile>(7, fence_texture()); });
        return factory;
    }

    auto TileFactory::with_burgundy_registry() -> TileFactory {
        auto factory = TileFactory {};
        factory.register_builder(0, [] { return std::make_unique<game::EmptyTile>(); });
        factory.register_builder(1, [] { return std::make_unique<game::TexturedWallTile>(1, velvet_brick_texture()); });
        factory.register_builder(2, [] { return std::make_unique<game::TexturedWallTile>(2, damask_texture()); });
        factory.register_builder(3, [] { return std::make_unique<game::TexturedWallTile>(3, brass_panel_texture()); });
        factory.register_builder(4, [] { return std::make_unique<game::TexturedWallTile>(4, kitchen_tile_texture()); });
        factory.register_builder(5, [] {
            return std::make_unique<game::EmissiveWallTile>(5,
                app::Color {.r = 240, .g = 90, .b = 40, .a = 255}, 0.5F);
        });
        factory.register_builder(6, [] { return std::make_unique<game::DoorTile>(6, vault_door_texture()); });
        factory.register_builder(7, [] { return std::make_unique<game::ThinWallTile>(7, bead_curtain_texture()); });
        factory.register_builder(8, [] {
            return std::make_unique<game::EmissiveWallTile>(8,
                app::Color {.r = 230, .g = 200, .b = 110, .a = 255}, 0.15F);
        });
        factory.register_builder(9, [] { return std::make_unique<game::DoorTile>(9, door_texture()); });
        return factory;
    }

    auto operator<<(std::ostream& stream, const TileFactory& factory) -> std::ostream& {
        stream << "TileFactory{registered=" << factory.registered_count() << "}";
        return stream;
    }
}
