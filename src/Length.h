#ifndef LENGTH_H
#define LENGTH_H

#include <cstdint>
#include <string>

enum class Length
{
    NEXT_6_BITS_ARE_LENGTH,
    ADD_BYTE_FOR_LENGTH,
    NEXT_4_BYTES_ARE_LENGTH,
    SPECIAL_8_BIT_INT,
    SPECIAL_16_BIT_INT,
    SPECIAL_32_BIT_INT,
    SPECIAL_COMPRESSED,
    UNKNOWN,
};

namespace length
{
Length fromByte(const char * mapped);
int get(char ** mapped, Length l);
std::string toString(Length length);
}

#endif
