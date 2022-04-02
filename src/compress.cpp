#include "compress.hpp"

#include <stdexcept>
#include <cstdint>

namespace PBO
{
    Compress::Compress()
    {

    }

    void Compress::Decode(std::vector<char> &dst, size_t length, std::ifstream &in)
    {
        if(length <= 0)
            return;
        
        int i, j, r, c;
        uint32_t csr, csum = 0;

        int flags;
        for(i=0; i < COMPRESS_BUFFER_SIZE - COMPRESS_FACTOR; i++ )
            text_buf[i] = ' ';

        r = COMPRESS_BUFFER_SIZE - COMPRESS_FACTOR;
        flags = 0;
        while(length > 0)
        {
            if(((flags >>= 1) & 256) == 0)
            {
                c = in.get();
                flags = c | 0xff00;
            }

            if (in.fail() || in.eof())
                throw std::out_of_range("Stream read failed");

            if(flags & 1)
            {
                c = in.get();
                if (in.fail() || in.eof())
                    throw std::out_of_range("Stream read failed");

                csum += c;

                // save byte
                dst.push_back(c);
                length--;

                // continue decompression
                text_buf[r] = static_cast<unsigned char>(c);
                r++;
                r &= (COMPRESS_BUFFER_SIZE - 1);
            }
            else
            {
                i = in.get();
                j = in.get();
                if (in.fail() || in.eof())
                    throw std::out_of_range("Stream read failed");

                i |= (j & 0xf0) << 4;
                j &= 0x0f;
                j += COMPRESS_THRESHOLD;

                int ii = r - i;
                int jj = j + ii;

                if (j + 1 > static_cast<int>(length))
                    throw std::overflow_error("LZW overflow");

                while(ii <= jj)
                {
                    c = static_cast<uint8_t>(text_buf[ii & (COMPRESS_BUFFER_SIZE - 1)]);
                    csum += c;

                    // save byte
                    dst.push_back(c);
                    length--;

                    // continue decompression
                    text_buf[r] = static_cast<unsigned char>(c);

                    r++;
                    r &= (COMPRESS_BUFFER_SIZE - 1);

                    ii++;
                }
            }
        }
        
        in.read(reinterpret_cast<char*>(&csr), sizeof(uint32_t));
        if (in.fail() || in.eof())
            throw std::out_of_range("Stream read failed");

        if(csr != csum)
            throw std::runtime_error("Checksum failed");
    }

    Compress::~Compress()
    {

    }
}