#include "RDBMmapReader.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <boost/endian/conversion.hpp>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Length.h"
#include "RDBType.h"
#include "Value.h"

RDBMmapReader::RDBMmapReader(std::string filename)
{
    int fd = ::open(filename.c_str(), O_RDONLY);

    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        close(fd);
        throw std::runtime_error("Error getting file size");
    }

    auto file_size = sb.st_size;

    auto * original_mmap = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    mapped = static_cast<char *>(original_mmap);
    if (mapped == MAP_FAILED)
    {
        close(fd);
        throw std::runtime_error("Error mapping file");
    }

    header.magic = std::string(&mapped[offset], 5);
    offset += 5;
    header.version = std::string(&mapped[offset], 4);
    offset += 4;
}

std::optional<std::unique_ptr<Token>> RDBMmapReader::next()
{
    if (finished)
    {
        return std::nullopt;
    }

    auto rdb_type = rdbtype::getBlock(&mapped[offset], offset);

    uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    switch (rdb_type)
    {
        case RDBType::AUXILIARY:
            return getAuxiliary();
        case RDBType::DATABASE_SELECTOR:
            return getDBSelector();
        case RDBType::RESIZE_DB:
            return getResizeDB();
        case RDBType::END_OF_FILE:
            std::memcpy(buf, &mapped[offset], 8);
            offset += 8;
            header.checksum = boost::endian::load_little_u64(buf);
            finished = true;
            return std::make_unique<Uint64Token>(Uint64Token{RDBType::END_OF_FILE, "CRC32", header.checksum});
        case RDBType::VALUE_TYPE:
            offset -= 1;
            return getKV(0);
        case RDBType::EXPIRE_TIME_MS:
            std::memcpy(buf, &mapped[offset], 8);
            offset += 8;
            return getKV(boost::endian::load_little_u64(buf));
        case RDBType::EXPIRE_TIME_SECONDS:
            std::memcpy(buf, &mapped[offset], 4);
            offset += 4;
            return getKV(boost::endian::load_little_u32(buf));
        case RDBType::UNEXPECTED:
            printf("Unexpected\n");
            return std::nullopt;
            break;
    }


    return std::nullopt;
}

std::optional<std::unique_ptr<KvToken>> RDBMmapReader::getKV(int64_t expiration_sec)
{
    const auto vt = valuetype::get(&mapped[offset], offset);
    Length length = Length::UNKNOWN;
    std::string key;
    std::string val;
    int n = 0;

    switch (vt)
    {
        case ValueType::STRING:
            length = length::fromByte(&mapped[offset]);
            key = readEncodedString(length);
            length = length::fromByte(&mapped[offset]);
            val = readEncodedString(length);
            return std::make_unique<KvToken>(KvToken{key, val, expiration_sec});
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
            printf("List quick list\n");
            length = length::fromByte(&mapped[offset]);
            key = readEncodedString(length);
            length = length::fromByte(&mapped[offset]);
            n = readIntEncodedAsString(length);
            offset += n;
            printf("List quick list: '%s'. Len: %d \n", key.c_str(), n);
            break;
        case ValueType::UNKNOWN:
            printf("ERROR: Unknown\n");
            break;
    }

    return std::make_unique<KvToken>(KvToken{key, val, expiration_sec});
}

std::optional<std::unique_ptr<Token>> RDBMmapReader::getResizeDB()
{
    auto length = length::fromByte(&mapped[offset]);
    auto data_length = length::get(mapped, offset, length);
    header.hash_table_size = data_length;

    length = length::fromByte(&mapped[offset]);
    data_length = length::get(mapped, offset, length);
    header.expiry_hash_table_used = data_length;

    return std::make_unique<ResizeDBToken>(ResizeDBToken{header.hash_table_size, header.expiry_hash_table_used});
}

std::optional<std::unique_ptr<Token>> RDBMmapReader::getDBSelector()
{
    int byte = static_cast<int>(static_cast<uint8_t>(mapped[offset]));
    header.database_selector = byte;
    offset += 1;
    return std::make_unique<IntToken>(IntToken{RDBType::DATABASE_SELECTOR, "db", byte});
}

std::optional<std::unique_ptr<Token>> RDBMmapReader::getAuxiliary()
{
    auto length = length::fromByte(&mapped[offset]);

    auto key = readEncodedString(length);
    std::string value;
    int n = 0;

    length = length::fromByte(&mapped[offset]);
    switch (length)
    {
        case Length::ADD_BYTE_FOR_LENGTH:
        case Length::NEXT_4_BYTES_ARE_LENGTH:
        case Length::NEXT_6_BITS_ARE_LENGTH:
            value = readEncodedString(length);
            break;
        case Length::SPECIAL_8_BIT_INT:
        case Length::SPECIAL_16_BIT_INT:
        case Length::SPECIAL_32_BIT_INT:
            n = readIntEncodedAsString(length);
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
        header.redis_version = value;
        return std::make_unique<StringToken>(StringToken{RDBType::AUXILIARY, key, value});
    }
    else if (key == "redis-bits")
    {
        header.bits = n;
    }
    else if (key == "ctime")
    {
        header.ctime = n;
    }
    else if (key == "used-mem")
    {
        header.used_memory = n;
    }
    else if (key == "aof-base")
    {
        header.aof_base = n;
    }
    else if (key == "aof-preamble")
    {
        header.aof_preamble = n;
    }

    return std::make_unique<IntToken>(IntToken{RDBType::AUXILIARY, key, n});
}

int RDBMmapReader::readIntEncodedAsString(Length length)
{
    int n = 0;
    unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    offset += 1;

    switch (length)
    {
        case Length::SPECIAL_8_BIT_INT:
            n = static_cast<int>(static_cast<uint8_t>(mapped[offset]));
            offset += 1;
            return n;
        case Length::SPECIAL_16_BIT_INT:
            std::memcpy(buf, &mapped[offset], 2);
            offset += 2;
            n = boost::endian::load_little_u16(buf);
            return n;
        case Length::SPECIAL_32_BIT_INT:
            std::memcpy(buf, &mapped[offset], 4);
            offset += 4;
            n = boost::endian::load_little_u32(buf);
            return n;
        case Length::NEXT_6_BITS_ARE_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(mapped[offset]));
            n = n & 0b00111111;
            offset += 1;
            return n;
        case Length::ADD_BYTE_FOR_LENGTH:
            n = static_cast<int>(static_cast<uint8_t>(mapped[offset]));
            buf[0] = n & 0b00111111;
            offset += 1;
            buf[1] = static_cast<uint8_t>(mapped[offset]);
            offset += 1;
            n = boost::endian::load_little_u16(buf);
            return n;
        case Length::NEXT_4_BYTES_ARE_LENGTH:
            std::memcpy(buf, &mapped[offset], 4);
            n = static_cast<int>(static_cast<uint8_t>(mapped[offset]));
            buf[0] = n & 0b00111111;
            offset += 4;
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

std::string RDBMmapReader::readEncodedString(Length length)
{
    const uint8_t type = static_cast<uint8_t>(mapped[offset]);
    uint8_t n;
    std::string result;
    int size;
    uint8_t nn[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    switch (length)
    {
        case Length::NEXT_6_BITS_ARE_LENGTH:
            n = type & 0b00111111;
            offset += 1;
            result = std::string(&mapped[offset], n);
            offset += n;
            return result;
        case Length::ADD_BYTE_FOR_LENGTH:
            n = type & 0b00111111;
            offset += 1;

            nn[0] = static_cast<uint8_t>(mapped[offset]);
            nn[1] = n;
            size = boost::endian::load_little_u64(nn);

            result = std::string(&mapped[offset], size);
            offset += size;
            return result;
        case Length::NEXT_4_BYTES_ARE_LENGTH:
            break;
        default:
            printf("Unknown length\n");
            break;
    }

    return "";
}

void RDBMmapReader::printHeaderInfo() const
{
    printf("Header:\n\tMagic: '%s'\n", header.magic.c_str());
    printf("\tVersion: '%s'\n", header.version.c_str());
    printf("\tRedis version: '%s'\n", header.redis_version.c_str());
    printf("\tBits: '%d'\n", header.bits);
    printf("\tCTime: '%d'\n", header.ctime);
    printf("\tUsed memory: '%d'\n", header.used_memory);
    printf("\tAOF base: '%d'\n", header.aof_base);
    printf("\tAOF preamble: '%d'\n", header.aof_preamble);
    printf("\tDatabase selector: '%d'\n", header.database_selector);
    printf("\tChecksum: '%lu'\n", header.checksum);
    printf("Resize DB:\n\tHash Table size: '%d'\n", header.hash_table_size);
    printf("\tExpiry Hash table size: '%d'\n", header.expiry_hash_table_used);
    printf("\tChecksum: '%lu'\n", header.checksum);
}

std::shared_ptr<RedisHeader> RDBMmapReader::getHeader()
{
    return std::make_shared<RedisHeader>(header);
}

RDBMmapReader::~RDBMmapReader()
{
    munmap(mapped, file_size);
    close(fd);
}
