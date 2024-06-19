#include <iostream>
#include <memory>
#include <optional>
#include <boost/endian/conversion.hpp>
#include "../src/RDB.h"
#include "../src/RDBMmapReader.h"

// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Iterator way", "[Iterator]")
{
    auto reader = std::make_unique<RDBMmapReader>("../dump.rdb");
    auto data = reader->next();

    StringToken * st;
    IntToken * it;
    Uint64Token * lt;
    ResizeDBToken * rt;
    KvToken * kvt;

    while (data.has_value())
    {
        switch (data->get()->type)
        {
            case RDBType::AUXILIARY:
                // Auxiliary can be String or Int
                st = dynamic_cast<StringToken *>(data->get());
                if (st != nullptr)
                {
                    std::printf("type: %s\t%s:%s\n", rdb::type::toString(st->type).c_str(), st->key.c_str(), st->value.c_str());
                    break;
                }

                it = dynamic_cast<IntToken *>(data->get());
                if (it != nullptr)
                {
                    std::printf("type: %s\t%s:%d\n", rdb::type::toString(it->type).c_str(), it->key.c_str(), it->value);
                    break;
                }
                break;
            case RDBType::RESIZE_DB:
                rt = static_cast<ResizeDBToken *>(data->get());
                printf(
                    "type: %s\n\tHash table size: %d:\n\tExpiry Hash table size:%d\n",
                    rdb::type::toString(rt->type).c_str(),
                    rt->hash_table_size,
                    rt->expiry_hash_table_size);
                break;
            case RDBType::DATABASE_SELECTOR:
                it = static_cast<IntToken *>(data->get());
                printf("type: %s\t%s:%d\n", rdb::type::toString(it->type).c_str(), it->key.c_str(), it->value);
                break;
            case RDBType::VALUE_TYPE:
            case RDBType::EXPIRE_TIME_MS:
            case RDBType::EXPIRE_TIME_SECONDS:
                kvt = static_cast<KvToken *>(data->get());
                if (kvt->expiration_sec > 0)
                {
                    printf(
                        "type: %s. Key: %s, Value: %s, Expiration: %lu\n",
                        rdb::type::toString(kvt->type).c_str(),
                        kvt->key.c_str(),
                        kvt->value.c_str(),
                        kvt->expiration_sec);
                }
                else
                {
                    printf("type: %s. Key: %s, Value: %s\n", rdb::type::toString(kvt->type).c_str(), kvt->key.c_str(), kvt->value.c_str());
                }
                break;
            case RDBType::END_OF_FILE:
                lt = static_cast<Uint64Token *>(data->get());
                printf("type: %s\t%s:%lu\n", rdb::type::toString(lt->type).c_str(), lt->key.c_str(), lt->value);
                break;
            default:
                printf("Type %s not implemented\n", rdb::type::toString(data->get()->type).c_str());
                break;
        }

        data = reader->next();
    }
    try
    {
        auto header = reader->getHeader();
        reader->printHeaderInfo();
        REQUIRE(header->magic == "REDIS");
        REQUIRE(header->version == "0011");
        REQUIRE(header->redis_version == "7.2.5");
        REQUIRE(header->bits == 64);
        REQUIRE(header->ctime == 1716905479);
        REQUIRE(header->used_memory == 478720);
        REQUIRE(header->aof_base == 0);
        REQUIRE(header->database_selector == 0);
        REQUIRE(header->hash_table_size == 3);
        REQUIRE(header->expiry_hash_table_used == 2);
        REQUIRE(header->checksum == 13314309817343659540ULL);
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        FAIL();
    }
}
