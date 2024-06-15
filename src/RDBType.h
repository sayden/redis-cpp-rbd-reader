#ifndef RDBTYPE_H
#define RDBTYPE_H

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
RDBType getBlock(char **);
}

#endif
