#include "Length.h"
#include <cstdint>
#include <string>
#include <boost/endian/conversion.hpp>

namespace length
{
Length fromByte(const char * mapped)
{
    const uint8_t type = static_cast<uint8_t>(*mapped);
    auto first_two = type & 0b11000000;
    switch (first_two)
    {
        case 0b00000000:
            return Length::NEXT_6_BITS_ARE_LENGTH;
        case 0b01000000:
            return Length::ADD_BYTE_FOR_LENGTH;
        case 0b10000000:
            return Length::NEXT_4_BYTES_ARE_LENGTH;
        case 0b11000000:
            switch (type)
            {
                case 0b11000000:
                    return Length::SPECIAL_8_BIT_INT;
                case 0b11000001:
                    return Length::SPECIAL_16_BIT_INT;
                case 0b11000010:
                    return Length::SPECIAL_32_BIT_INT;
                case 0b11000011:
                    return Length::SPECIAL_COMPRESSED;
                default:
                    return Length::UNKNOWN;
            };
        default:
            return Length::UNKNOWN;
    }
}

std::string toString(Length length)
{
    switch (length)
    {
        case Length::NEXT_6_BITS_ARE_LENGTH:
            return "NEXT_6_BITS_ARE_LENGTH";
        case Length::ADD_BYTE_FOR_LENGTH:
            return "ADD_BYTE_FOR_LENGTH";
        case Length::NEXT_4_BYTES_ARE_LENGTH:
            return "NEXT_4_BYTES_ARE_LENGTH";
        case Length::SPECIAL_8_BIT_INT:
            return "SPECIAL_8_BIT_INT";
        case Length::SPECIAL_16_BIT_INT:
            return "SPECIAL_16_BIT_INT";
        case Length::SPECIAL_32_BIT_INT:
            return "SPECIAL_32_BIT_INT";
        case Length::SPECIAL_COMPRESSED:
            return "SPECIAL_COMPRESSED";
        case Length::UNKNOWN:
            return "UNKNOWN";
    }

    return "UNKNOWN";
}

int get(const char * mapped, uint64_t & offset, Length l)
{
    unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int n = 0;

    switch (l)
    {
        case Length::NEXT_6_BITS_ARE_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(mapped[offset]) & 0b00111111);
            offset += 1;
            return n;
        case Length::ADD_BYTE_FOR_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(mapped[offset]) & 0b00111111);
            offset += 1;
            buf[1] = n;
            buf[0] = static_cast<uint8_t>(mapped[offset]);
            return boost::endian::load_little_u16(buf);
        case Length::NEXT_4_BYTES_ARE_LENGTH:
            offset += 1;
            std::memcpy(buf, &mapped[offset], 4);
            offset += 4;
            return boost::endian::load_little_u32(buf);
        case Length::SPECIAL_8_BIT_INT:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::SPECIAL_16_BIT_INT:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::SPECIAL_32_BIT_INT:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::SPECIAL_COMPRESSED:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::UNKNOWN:
            printf("Length: %s\n", toString(l).c_str());
            return 0;
    }

    return 0;
}

int get(char ** mapped, Length l)
{
    unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int n = 0;

    switch (l)
    {
        case Length::NEXT_6_BITS_ARE_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(**mapped) & 0b00111111);
            *mapped += 1;
            return n;
        case Length::ADD_BYTE_FOR_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(**mapped) & 0b00111111);
            *mapped += 1;
            buf[1] = n;
            buf[0] = static_cast<uint8_t>(**mapped);
            return boost::endian::load_little_u16(buf);
        case Length::NEXT_4_BYTES_ARE_LENGTH:
            *mapped += 1;
            std::memcpy(buf, *mapped, 4);
            *mapped += 4;
            return boost::endian::load_little_u32(buf);
        case Length::SPECIAL_8_BIT_INT:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::SPECIAL_16_BIT_INT:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::SPECIAL_32_BIT_INT:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::SPECIAL_COMPRESSED:
            printf("Length: %s\n", toString(l).c_str());
            break;
        case Length::UNKNOWN:
            printf("Length: %s\n", toString(l).c_str());
            return 0;
    }

    return 0;
}
}
