#pragma once

#include <cstdint>
#include <limits>

namespace lucy {

using Entity = uint32_t;

namespace entity {
    constexpr uint32_t INDEX_BITS = 20;
    constexpr uint32_t GEN_BITS = 12;
    constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1;
    constexpr uint32_t GEN_MASK = (1u << GEN_BITS) - 1;
    constexpr uint32_t MAX_INDEX = INDEX_MASK;
    constexpr uint32_t MAX_GEN = GEN_MASK;

    inline constexpr Entity null = std::numeric_limits<uint32_t>::max();

    inline constexpr uint32_t index(Entity e) { return e & INDEX_MASK; }
    inline constexpr uint32_t generation(Entity e) { return (e >> INDEX_BITS) & GEN_MASK; }
    inline constexpr Entity make(uint32_t index, uint32_t gen) {
        return (gen << INDEX_BITS) | index;
    }
}

}
