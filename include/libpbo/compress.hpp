#pragma once

#include <fstream>
#include <vector>

#define COMPRESS_BUFFER_SIZE    4096
#define COMPRESS_FACTOR         18
#define COMPRESS_THRESHOLD      2

namespace PBO
{
    class Compress
    {
    public:
        Compress();
        ~Compress();

        void Decode(std::vector<char> &dst, size_t length, std::ifstream &in);
    private:
        unsigned char text_buf[COMPRESS_BUFFER_SIZE + COMPRESS_FACTOR - 1];
    };
}