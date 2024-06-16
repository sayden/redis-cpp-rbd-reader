#include "RDBReader.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <boost/endian/conversion.hpp>
#include <sys/mman.h>
#include <sys/stat.h>
#include "Length.h"
#include "RDBType.h"
#include "Value.h"

namespace reader
{

std::string readEncodedString(char ** mapped, Length length)
{
    const uint8_t type = static_cast<uint8_t>(**mapped);
    uint8_t n;
    std::string result;
    int size;
    uint8_t nn[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    switch (length)
    {
        case Length::NEXT_6_BITS_ARE_LENGTH:
            n = type & 0b00111111;
            *mapped += 1;
            result = std::string(*mapped, n);
            *mapped += n;
            return result;
        case Length::ADD_BYTE_FOR_LENGTH:
            n = type & 0b00111111;
            *mapped += 1;

            nn[0] = *mapped[0];
            nn[1] = n;
            size = boost::endian::load_little_u64(nn);

            result = std::string(*mapped, size);
            mapped += size;
            return result;
        case Length::NEXT_4_BYTES_ARE_LENGTH:
            break;
        default:
            printf("Unknown length\n");
            break;
    }

    return "";
}

int readIntEncodedAsString(char ** mapped, Length length)
{
    int n = 0;
    unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    *mapped += 1;
    switch (length)
    {
        case Length::SPECIAL_8_BIT_INT:
            n = static_cast<int>(static_cast<uint8_t>(**mapped));
            *mapped += 1;
            return n;
        case Length::SPECIAL_16_BIT_INT:
            std::memcpy(buf, *mapped, 2);
            *mapped += 2;
            n = boost::endian::load_little_u16(buf);
            return n;
        case Length::SPECIAL_32_BIT_INT:
            std::memcpy(buf, *mapped, 4);
            *mapped += 4;
            n = boost::endian::load_little_u32(buf);
            return n;
        case Length::NEXT_6_BITS_ARE_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(**mapped));
            n = n & 0b00111111;
            *mapped += 1;
            return n;
        case Length::ADD_BYTE_FOR_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(**mapped));
            buf[0] = n & 0b00111111;
            *mapped += 1;
            buf[1] = static_cast<uint8_t>(**mapped);
            *mapped += 1;
            n = boost::endian::load_little_u16(buf);
            return n;
        case Length::NEXT_4_BYTES_ARE_LENGTH:
            std::memcpy(buf, *mapped, 4);
            n = static_cast<int>(static_cast<uint8_t>(**mapped));
            buf[0] = n & 0b00111111;
            *mapped += 4;
            n = boost::endian::load_little_u32(buf);
            return n;
        case Length::SPECIAL_COMPRESSED:
            printf("Special compressed\n");
            return 0;
        case Length::UNKNOWN:
            printf("Unknown\n");
            return 0;
    }
    return 0;
}


void printResizeDB(char ** mapped, RedisHeader & in)
{
    auto length = length::fromByte(*mapped);
    auto length_data = length::get(mapped, length);
    in.hash_table_size = length_data;

    length = length::fromByte(*mapped);
    length_data = length::get(mapped, length);
    in.expiry_hash_table_used = length_data;
}

void printAuxiliary(char ** mapped, RedisHeader * in)
{
    auto length = length::fromByte(*mapped);

    auto key = readEncodedString(mapped, length);
    std::string value;
    int n = 0;

    length = length::fromByte(*mapped);
    switch (length)
    {
        case Length::ADD_BYTE_FOR_LENGTH:
        case Length::NEXT_4_BYTES_ARE_LENGTH:
        case Length::NEXT_6_BITS_ARE_LENGTH:
            value = readEncodedString(mapped, length);
            break;
        case Length::SPECIAL_8_BIT_INT:
        case Length::SPECIAL_16_BIT_INT:
        case Length::SPECIAL_32_BIT_INT:
            n = readIntEncodedAsString(mapped, length);
            break;
        case Length::SPECIAL_COMPRESSED:
            printf("Special compressed\n");
            break;
        case Length::UNKNOWN:
            printf("Unknown\n");
            break;
    }

    if (key == "redis-ver")
    {
        in->redis_version = value;
    }
    else if (key == "redis-bits")
    {
        in->bits = n;
    }
    else if (key == "ctime")
    {
        in->ctime = n;
    }
    else if (key == "used-mem")
    {
        in->used_memory = n;
    }
    else if (key == "aof-base")
    {
        in->aof_base = n;
    }
    else if (key == "aof-preamble")
    {
        in->aof_preamble = n;
    }
}


void printKV(char ** mapped, int expiration_sec)
{
    uint64_t ms = static_cast<uint64_t>(expiration_sec);
    printKV(mapped, ms);
}

void printKV(char ** mapped, uint64_t expiration_sec)
{
    const auto vt = valuetype::get(mapped);
    Length length = Length::UNKNOWN;
    std::string key;
    std::string val;
    int n = 0;

    switch (vt)
    {
        case ValueType::STRING:
            length = length::fromByte(*mapped);
            key = readEncodedString(mapped, length);
            length = length::fromByte(*mapped);
            val = readEncodedString(mapped, length);
            if (expiration_sec > 0)
            {
                printf("Key: %s, Value: %s, Expiration: %lu\n", key.c_str(), val.c_str(), expiration_sec);
            }
            else
            {
                printf("Key: %s, Value: %s\n", key.c_str(), val.c_str());
            }
            break;
        case ValueType::LIST:
            printf("List\n");
            break;
        case ValueType::SET:
            printf("Set\n");
            break;
        case ValueType::SORTED_SET:
            printf("Sorted set\n");
            break;
        case ValueType::HASH:
            printf("Hash\n");
            break;
        case ValueType::ZIP_MAP:
            printf("Zip map\n");
            break;
        case ValueType::ZIP_LIST:
            printf("Zip list\n");
            break;
        case ValueType::INT_SET:
            printf("Int set\n");
            break;
        case ValueType::STREAM:
            printf("Stream\n");
            break;
        case ValueType::SORTED_SET_ZIP_LIST:
            printf("Sorted set zip list\n");
            break;
        case ValueType::HASH_ZIP_LIST:
            printf("Hash zip list\n");
            break;
        case ValueType::LIST_QUICK_LIST:
            length = length::fromByte(*mapped);
            key = readEncodedString(mapped, length);
            length = length::fromByte(*mapped);
            n = readIntEncodedAsString(mapped, length);
            mapped += n;
            printf("List quick list: '%s'. Len: %d \n", key.c_str(), n);
            break;
        case ValueType::UNKNOWN:
            printf("ERROR: Unknown\n");
            break;
    }
}

RDBReader::RDBReader(const std::string filename) : filename(filename)
{
    std::cout << "filename: " << filename << std::endl;
}

std::unique_ptr<RedisHeader> reader::RDBReader::readHeader() const
{
    auto in = std::make_unique<RedisHeader>();
    int fd = ::open(filename.c_str(), O_RDONLY);

    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        close(fd);
        throw std::runtime_error("Error getting file size");
    }

    auto file_size = sb.st_size;
    char * mapped = static_cast<char *>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapped == MAP_FAILED)
    {
        close(fd);
        throw std::runtime_error("Error mapping file");
    }

    in->magic = std::string(mapped, 5);
    mapped += 5;
    in->version = std::string(mapped, 4);
    mapped += 4;

    RDBType rdb_type;
    int n;
    uint8_t byte = 0;
    uint64_t ms;
    uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    // while (true)
    while (rdb_type != RDBType::END_OF_FILE)
    {
        rdb_type = rdbtype::getBlock(&mapped);

        switch (rdb_type)
        {
            case RDBType::AUXILIARY:
                reader::printAuxiliary(&mapped, in.get());
                break;
            case RDBType::DATABASE_SELECTOR:
                byte = static_cast<int>(static_cast<uint8_t>(*mapped));
                in->database_selector = byte;
                mapped += 1;
                break;
            case RDBType::RESIZE_DB:
                reader::printResizeDB(&mapped, *in);
                break;
            case RDBType::END_OF_FILE:
                break;
            case RDBType::VALUE_TYPE:
                mapped -= 1;
                reader::printKV(&mapped, 0);
                break;
            case RDBType::EXPIRE_TIME_MS:
                std::memcpy(buf, mapped, 8);
                mapped += 8;
                ms = boost::endian::load_little_u64(buf);
                reader::printKV(&mapped, ms);
                break;
            case RDBType::EXPIRE_TIME_SECONDS:
                std::memcpy(buf, mapped, 4);
                mapped += 4;
                n = boost::endian::load_little_u32(buf);
                reader::printKV(&mapped, n * 1000);
                break;
            case RDBType::UNEXPECTED:
                printf("Unexpected\n");
                break;
        }
    }

    std::memcpy(buf, mapped, 8);
    mapped += 8;
    in->checksum = boost::endian::load_little_u64(buf);
    printf("Checksum: %lu\n", in->checksum);

    munmap(mapped, file_size);

    return in;
}

void RDBReader::printHeaderInfo(RedisHeader * in) const
{
    std::cout << "filename: " << filename << std::endl;

    printf("Header:\n\tMagic: '%s'\n", in->magic.c_str());
    printf("\tVersion: '%s'\n", in->version.c_str());
    printf("\tRedis version: '%s'\n", in->redis_version.c_str());
    printf("\tBits: '%d'\n", in->bits);
    printf("\tCTime: '%d'\n", in->ctime);
    printf("\tUsed memory: '%d'\n", in->used_memory);
    printf("\tAOF base: '%d'\n", in->aof_base);
    printf("\tAOF preamble: '%d'\n", in->aof_preamble);
    printf("\tDatabase selector: '%d'\n", in->database_selector);
    printf("\tChecksum: '%lu'\n", in->checksum);
    printf("Resize DB:\n\tHash Table size: '%d'\n", in->hash_table_size);
    printf("\tExpiry Hash table size: '%d'\n", in->expiry_hash_table_used);
}
}
