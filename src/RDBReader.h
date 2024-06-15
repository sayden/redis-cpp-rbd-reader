#ifndef RDBREADER_H
#define RDBREADER_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include "Length.h"
#include "RedisHeader.h"

class RDBReader
{
public:
    explicit RDBReader(std::string filename);
    void printHeaderInfo(RedisHeader & in) const;
    std::shared_ptr<RedisHeader> readHeader(std::unique_ptr<RedisHeader> in) const;

    int fd;
    std::string filename;
};

namespace rdbreader
{
std::string readEncodedString(char ** mapped, Length length);
int readIntEncodedAsString(char * mapped, Length length);

void printAuxiliary(char ** mapped);
void printResizeDB(char ** mapped);
void printKV(char ** mapped, int expiration_sec);
void printKV(char ** mapped, uint64_t expiration_sec);

}

#endif
