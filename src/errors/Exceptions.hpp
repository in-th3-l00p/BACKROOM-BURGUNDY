#pragma once

#include <exception>
#include <string>

namespace escape::errors {
    class EscapeError : public std::exception {
    public:
        explicit EscapeError(std::string message) : message_(std::move(message)) {}

        auto what() const noexcept -> const char* override { return message_.c_str(); }

    private:
        std::string message_;
    };

    class MapLoadError : public EscapeError {
    public:
        using EscapeError::EscapeError;
    };

    class TextureLoadError : public EscapeError {
    public:
        using EscapeError::EscapeError;
    };

    class OutOfBoundsError : public EscapeError {
    public:
        using EscapeError::EscapeError;
    };

    class InvalidConfigError : public EscapeError {
    public:
        using EscapeError::EscapeError;
    };
}
