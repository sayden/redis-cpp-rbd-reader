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

class Token
{
public:
    explicit Token(RDBType type) : type(type) { }
    RDBType type;
    virtual ~Token() = default;
};

class Uint64Token : public Token
{
public:
    Uint64Token(RDBType type, std::string key, uint64_t value) : Token(type), key(std::move(key)), value(value) { }
    std::string key;
    uint64_t value;

    ~Uint64Token() override = default;
};

class IntToken : public Token
{
public:
    IntToken(RDBType type, std::string key, int value) : Token(type), key(std::move(key)), value(value) { }
    std::string key;
    int value;

    ~IntToken() override = default;
};

class ResizeDBToken : public Token
{
public:
    ResizeDBToken(int hash_table_size, int expiry_hash_table_size)
        : Token(RDBType::RESIZE_DB), hash_table_size(hash_table_size), expiry_hash_table_size(expiry_hash_table_size)
    {
    }

    int hash_table_size;
    int expiry_hash_table_size;

    ~ResizeDBToken() override = default;
};

class StringToken : public Token
{
public:
    StringToken(RDBType type, std::string & key, std::string & value) : Token(type), key(std::move(key)), value(std::move(value)) { }
    std::string key;
    std::string value;

    ~StringToken() override = default;
};

class KvToken : public Token
{
public:
    KvToken(std::string & key, std::string & value, int64_t expiration_sec)
        : Token(RDBType::VALUE_TYPE), key(key), value(value), expiration_sec(expiration_sec)
    {
    }
    std::string key;
    std::string value;
    int64_t expiration_sec;

    ~KvToken() override = default;
};

namespace rdbtype
{
std::string toString(RDBType);
RDBType getBlock(char **);
RDBType getBlock(const char * type, uint64_t & offset);
}

#endif
