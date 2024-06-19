#ifndef REDIS_HEADER_H
#define REDIS_HEADER_H

#include <cstdint>
#include <string>

struct RedisHeader
{
    std::string magic;
    std::string version;
    std::string redis_version;
    uint8_t bits = 0;
    uint32_t ctime = 0;
    uint32_t used_memory = 0;
    uint8_t aof_base = 0;
    uint8_t database_selector = 0;

    int hash_table_size = 0;
    int expiry_hash_table_used = 0;

    uint64_t checksum = 0;
    uint32_t aof_preamble = 0;
};

#endif
