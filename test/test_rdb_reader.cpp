#include <cstdint>
#include <iostream>
#include <memory>
#include <boost/endian/conversion.hpp>
#include "../src/RDBReader.h"

// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("RDBReader constructor", "[RDBReader]")
{
    RDBReader rdb_reader("../dump.rdb");
    try
    {
        auto in = rdb_reader.readHeader(std::make_unique<RedisHeader>());
        REQUIRE(in->magic == "REDIS");
        REQUIRE(in->version == "0011");
        REQUIRE(in->bits == 64);
        REQUIRE(in->ctime == 1716905479);
        REQUIRE(in->used_memory == 478720);
        REQUIRE(in->aof_base == 0);
        REQUIRE(in->database_selector == 0);
        REQUIRE(in->hash_table_size == 3);
        REQUIRE(in->expiry_hash_table_used == 2);
        REQUIRE(in->checksum == 13314309817343659540ULL);
        rdb_reader.printHeaderInfo(*in);
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        FAIL();
    }
}
