#include "core/base64.h"

#include "core/assert.h"
#include "core/cint.h"

#include <sstream>
#include <string_view>
#include <array>

// source: https://en.wikipedia.org/wiki/Base64

namespace euphoria::core::base64
{
    namespace
    {
        constexpr std::string_view codes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    }

    std::string
    encode(std::shared_ptr<memory_chunk> memory)
    {
        memory_chunk& in = *memory;
        std::ostringstream out;

        for(int i = 0; i < in.get_size(); i += 3)
        {
            int b = (in[i] & 0xFC) >> 2;
            out << codes[b];
            b = (in[i] & 0x03) << 4;
            if(i + 1 < in.get_size())
            {
                b |= (in[i + 1] & 0xF0) >> 4;
                out << codes[b];
                b = (in[i + 1] & 0x0F) << 2;
                if(i + 2 < in.get_size())
                {
                    b |= (in[i + 2] & 0xC0) >> 6;
                    out << codes[b];
                    b = in[i + 2] & 0x3F;
                    out << codes[b];
                }
                else
                {
                    out << codes[b];
                    out << '=';
                }
            }
            else
            {
                out << codes[b];
                out << "==";
            }
        }

        return out.str();
    }

    std::shared_ptr<memory_chunk>
    decode(const std::string& input)
    {
        if(input.length() % 4 == 0)
        {
            ASSERT(!input.empty());
            auto asize = (input.length() * 3) / 4;
            auto found = input.find('=') != std::string::npos;
            auto bsize = found ? (input.length() - input.find('=')) : 0;
            int size = c_sizet_to_int(asize) - c_sizet_to_int(bsize);

            ASSERT(size > 0);

            auto ret = memory_chunk::allocate(size);
            memory_chunk& decoded = *ret;
            int j = 0;
            for(int i = 0; i < c_sizet_to_int(input.size()); i += 4)
            {
                // This could be made faster (but more complicated) by precomputing these index locations.
                const auto b = std::array<size_t, 4>
                {
                        codes.find(input[c_int_to_sizet(i)]),
                        codes.find(input[c_int_to_sizet(i + 1)]),
                        codes.find(input[c_int_to_sizet(i + 2)]),
                        codes.find(input[c_int_to_sizet(i + 3)])
                };
                decoded[j++] = static_cast<char>((b[0] << 2) | (b[1] >> 4));
                if(b[2] < 64)
                {
                    decoded[j++] = static_cast<char>((b[1] << 4) | (b[2] >> 2));
                    if(b[3] < 64)
                    {
                        decoded[j++] = static_cast<char>((b[2] << 6) | b[3]);
                    }
                }
            }
            return ret;
        }
        else
        {
            return memory_chunk::null();
        }
    }
}
