#ifndef RDBMMAPREADER_H
#define RDBMMAPREADER_H

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include "Length.h"
#include "RDBType.h"
#include "RedisHeader.h"

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


class RDBMmapReader
{
public:
    explicit RDBMmapReader(std::string filename);
    ~RDBMmapReader();

    std::optional<std::unique_ptr<Token>> next();
    std::shared_ptr<RedisHeader> getHeader();
    void printHeaderInfo() const;

    bool finished = false;

private:
    char * mapped;

    long file_size;
    int fd;
    RedisHeader header;
    uint64_t offset = 0;

    std::optional<std::unique_ptr<Token>> getAuxiliary();
    std::optional<std::unique_ptr<Token>> getDBSelector();
    std::optional<std::unique_ptr<Token>> getResizeDB();
    std::optional<std::unique_ptr<KvToken>> getKV(int64_t expiration_sec);

    std::string readEncodedString(Length length);
    int readIntEncodedAsString(Length length);
};

#endif
