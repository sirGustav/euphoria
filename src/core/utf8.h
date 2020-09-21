#ifndef EUPHORIA_CORE_UTF8_H
#define EUPHORIA_CORE_UTF8_H

#include <cstddef>
#include <vector>

namespace euphoria::core
{
    template<typename String, typename OnCodepoint>
    bool Utf8ToCodepoints(const String& string, OnCodepoint on_codepoint)
    {
        // reference: https://en.wikipedia.org/wiki/UTF-8
        using Int = unsigned int;
        auto ToByte = [](char c)->std::byte { return static_cast<std::byte>(c); };

        constexpr std::byte mask0 = std::byte{0b10000000}; constexpr std::byte bit0 = std::byte{0b00000000};
        constexpr std::byte mask1 = std::byte{0b11000000}; constexpr std::byte bit1 = std::byte{0b10000000};
        constexpr std::byte mask2 = std::byte{0b11100000}; constexpr std::byte bit2 = std::byte{0b11000000};
        constexpr std::byte mask3 = std::byte{0b11110000}; constexpr std::byte bit3 = std::byte{0b11100000};
        constexpr std::byte mask4 = std::byte{0b11111000}; constexpr std::byte bit4 = std::byte{0b11110000};

        unsigned int bits  = 0;
        std::vector<std::byte> buffer;

        for(auto c: string)
        {
            const auto b = ToByte(c);
            if(bits == 0)
            {
                if( (mask0 & b) == bit0)
                {
                    on_codepoint(std::to_integer<Int>(b));
                }
                else
                {
                    buffer.push_back(b);
                    if((mask4 & b) == bit4)
                    {
                        bits = 4;
                    }
                    else if((mask3 & b) == bit3)
                    {
                        bits = 3;
                    }
                    else if((mask2 & b) == bit2)
                    {
                        bits = 2;
                    }
                    else if((mask1 & b) == bit1)
                    {
                        // stray continuation byte...
                        return false;
                    }
                    else
                    {
                        // implementation error?
                        return false;
                    }
                }
            }
            else
            {
                if((mask1 & b) != bit1)
                {
                    // invalid continuation bit
                    return false;
                }
                buffer.push_back(b);
                if(buffer.size() == bits)
                {
                    switch(bits)
                    {
                    case 2:
                        on_codepoint(
                            ( std::to_integer<Int>(buffer[0]&~mask2) << 6) |
                            ( std::to_integer<Int>(buffer[1]&~mask1) << 0)
                        );
                        break;
                    case 3:
                        on_codepoint(
                            ( std::to_integer<Int>(buffer[0]&~mask3) << 12) |
                            ( std::to_integer<Int>(buffer[1]&~mask1) << 6 ) |
                            ( std::to_integer<Int>(buffer[2]&~mask1) << 0 )
                        );
                        break;
                    case 4:
                        on_codepoint(
                            ( std::to_integer<Int>(buffer[0]&~mask4) << 18) |
                            ( std::to_integer<Int>(buffer[1]&~mask1) << 12) |
                            ( std::to_integer<Int>(buffer[2]&~mask1) << 6 ) |
                            ( std::to_integer<Int>(buffer[3]&~mask1) << 0 )
                        );
                        break;
                    default:
                        // unhandled size
                        return false;
                    }

                    bits = 0;
                    buffer.resize(0);
                }
            }
        }

        // if bits != 0 this means a unfinished codepoint
        return bits == 0;
    }
}

#endif  // EUPHORIA_CORE_UTF8_H