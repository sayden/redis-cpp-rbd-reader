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

    uint64_t file_size;
    int fd;
    RedisHeader header;
    uint64_t offset = 0;

    std::optional<std::unique_ptr<Token>> getAuxiliary();
    std::optional<std::unique_ptr<Token>> getDBSelector();
    std::optional<std::unique_ptr<Token>> getResizeDB();
    std::optional<std::unique_ptr<KvToken>> getKV(int64_t expiration_sec);
    void offsetAdvance(long length);

    std::string readEncodedString(Length length);
    int readIntEncodedAsString(Length length);
};

#endif
