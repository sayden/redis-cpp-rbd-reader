#ifndef REDIS_HEADER_H
#define REDIS_HEADER_H

#include <cstdint>
#include <string>

struct RedisHeader
{
    std::string magic;
    std::string version;
    std::string redis_version;
    uint8_t bits;
    uint32_t ctime;
    uint32_t used_memory;
    uint8_t aof_base;
    uint8_t database_selector;
    uint32_t hash_table_size;
    uint32_t expiry_hash_table_used;
    uint64_t checksum;
};

#endif
