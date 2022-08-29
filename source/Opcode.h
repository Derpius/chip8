#pragma once

#include <cstdint>

struct Opcode
{
    uint16_t raw;

    inline uint8_t Type() { return (raw & 0xF000) >> 12; }
    inline uint8_t X() { return (raw & 0x0F00) >> 8; }
    inline uint8_t Y() { return (raw & 0x00F0) >> 4; }

    template<int size>
    uint16_t N()
    {
        static_assert(size >= 1 && size <= 3, "Invalid opcode data size");
        if constexpr (size == 1)
            return raw & 0x000F;
        else if constexpr (size == 2)
            return raw & 0x00FF;
        else
            return raw & 0x0FFF;
    }
};
