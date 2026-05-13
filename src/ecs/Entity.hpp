#pragma once

#include <cstdint>

namespace escape::ecs {
    using Entity = std::uint32_t;

    inline constexpr auto null_entity = Entity {0};
}
