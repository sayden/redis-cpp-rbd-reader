#include "RDBType.h"
#include <cstdint>
#include <string>

namespace rdbtype
{

RDBType getBlock(const char * type, uint64_t & offset)
{
    auto mapped_type = RDBType::UNEXPECTED;
    offset += 1;
    uint8_t temp = static_cast<uint8_t>(*type);

    switch (temp)
    {
        case 0xFE:
            mapped_type = RDBType::DATABASE_SELECTOR;
            break;
        case 0xFD:
            mapped_type = RDBType::EXPIRE_TIME_SECONDS;
            break;
        case 0xFC:
            mapped_type = RDBType::EXPIRE_TIME_MS;
            break;
        case 0xFF:
            mapped_type = RDBType::END_OF_FILE;
            break;
        case 0xFA:
            mapped_type = RDBType::AUXILIARY;
            break;
        case 0xFB:
            mapped_type = RDBType::RESIZE_DB;
            break;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            mapped_type = RDBType::VALUE_TYPE;
            break;
        default:
            mapped_type = RDBType::UNEXPECTED;
            break;
    }

    return mapped_type;
}

std::string toString(RDBType type)
{
    switch (type)
    {
        case RDBType::AUXILIARY:
            return "AUXILIARY";
        case RDBType::DATABASE_SELECTOR:
            return "DATABASE_SELECTOR";
        case RDBType::END_OF_FILE:
            return "END_OF_FILE";
        case RDBType::EXPIRE_TIME_MS:
            return "EXPIRE_TIME_MS";
        case RDBType::EXPIRE_TIME_SECONDS:
            return "EXPIRE_TIME_SECONDS";
        case RDBType::RESIZE_DB:
            return "RESIZE_DB";
        case RDBType::VALUE_TYPE:
            return "VALUE_TYPE";
        case RDBType::UNEXPECTED:
            return "UNEXPECTED";
    }

    return "UNEXPECTED";
}

RDBType getBlock(char ** mapped)
{
    auto mapped_type = RDBType::UNEXPECTED;
    uint8_t type = static_cast<uint8_t>(**mapped);
    *mapped += 1;
    switch (type)
    {
        case 0xFE:
            mapped_type = RDBType::DATABASE_SELECTOR;
            break;
        case 0xFD:
            mapped_type = RDBType::EXPIRE_TIME_SECONDS;
            break;
        case 0xFC:
            mapped_type = RDBType::EXPIRE_TIME_MS;
            break;
        case 0xFF:
            mapped_type = RDBType::END_OF_FILE;
            break;
        case 0xFA:
            mapped_type = RDBType::AUXILIARY;
            break;
        case 0xFB:
            mapped_type = RDBType::RESIZE_DB;
            break;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            mapped_type = RDBType::VALUE_TYPE;
            break;
        default:
            mapped_type = RDBType::UNEXPECTED;
            break;
    }

    return mapped_type;
}
}
