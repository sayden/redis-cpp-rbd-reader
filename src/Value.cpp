#include "Value.h"
#include <cstdint>

namespace valuetype
{

ValueType get(const char * mapped, uint64_t & offset)
{
    auto c = static_cast<uint8_t>(mapped[offset]);
    offset += 1;
    switch (c)
    {
        case 0x00:
            return ValueType::STRING;
        case 0x01:
            return ValueType::LIST;
        case 0x02:
            return ValueType::SET;
        case 0x03:
            return ValueType::SORTED_SET;
        case 0x04:
            return ValueType::HASH;
        case 0x09:
            return ValueType::ZIP_MAP;
        case 0x0a:
            return ValueType::ZIP_LIST;
        case 0x0b:
            return ValueType::INT_SET;
        case 0x0c:
            return ValueType::SORTED_SET_ZIP_LIST;
        case 0x0d:
            return ValueType::HASH_ZIP_LIST;
        case 0x0e:
            return ValueType::LIST_QUICK_LIST;
        default:
            return ValueType::UNKNOWN;
    }
}

}
