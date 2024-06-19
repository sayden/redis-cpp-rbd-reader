#ifndef RDBTYPE_H
#define RDBTYPE_H

#include <cstdint>
#include <string>
enum class RDBType
{
    AUXILIARY,
    DATABASE_SELECTOR,
    END_OF_FILE,
    EXPIRE_TIME_MS,
    EXPIRE_TIME_SECONDS,
    RESIZE_DB,
    VALUE_TYPE,
    UNEXPECTED,
};

namespace rdbtype
{
std::string toString(RDBType);
RDBType getBlock(char **);
RDBType getBlock(const char * type, uint64_t & offset);
}

#endif
