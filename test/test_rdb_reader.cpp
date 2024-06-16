#include <iostream>
#include <memory>
#include <boost/endian/conversion.hpp>
#include "../src/RDB.h"

// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

void AddOne(std::shared_ptr<int> input)
{
    *input += 1;
}

TEST_CASE("RDBReader constructor", "[RDBReader]")
{
    rdb::reader::RDBReader rdb_reader("../dump.rdb");
    try
    {
        auto in = rdb_reader.readHeader();
        rdb_reader.printHeaderInfo(in.get());
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
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        FAIL();
    }
}

TEST_CASE("SmartPointers stuff", "[UniquePtr]")
{
    auto p = std::make_shared<int>(42);
    REQUIRE(*p == 42);

    AddOne(p);
    REQUIRE(*p == 43);
}
