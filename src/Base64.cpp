#include <stdexcept>
#include <stdint.h>
#include <cmath>
#include <cassert>
#include <cstring>
#include "../include/Base64.h"

namespace Base64
{

static uint8_t indexToChar(uint index)
{
    if (index <= 25)
        return 'A' + index;
    else if (index <= 51)
        return 'a' + (index-26);
    else if (index <= 61)
        return '0' + (index-52);
    else if (index == 62)
        return '+';
    else if (index == 63)
        return '/';

    assert(false);
    throw std::runtime_error{""};
}

std::string encode(const byteArray_t& input, bool applyPadding/*=true*/)
{
    std::string output;

    const size_t end = std::ceil(input.size()/3.f);
    for (size_t i{}; i < end; i++)
    {
        Sequence seq{};
        if (i*3+2 < input.size())
            memcpy((uint8_t*)(&seq)+0, input.data()+i*3+2, 1);
        if (i*3+1 < input.size())
            memcpy((uint8_t*)(&seq)+1, input.data()+i*3+1, 1);
        memcpy((uint8_t*)(&seq)+2, input.data()+i*3+0, 1);

        output += indexToChar(seq.fourth);
        output += indexToChar(seq.third);

        if (seq.second != 0 || i != end-1)
            output += indexToChar(seq.second);
        else if (applyPadding)
            output += BASE64_PADDING_CHAR;

        if (seq.first != 0 || i != end-1)
            output += indexToChar(seq.first);
        else if (applyPadding)
            output += BASE64_PADDING_CHAR;
    }

    return output;
}

static uint charToIndex(uint8_t ch)
{
    if (ch >= 'A' && ch <= 'Z')
        return 0 + ch - 'A';
    else if (ch >= 'a' && ch <= 'z')
        return 26 + ch - 'a';
    else if (ch >= '0' && ch <= '9')
        return 52 + ch - '0';
    else if (ch == '+')
        return 62;
    else if (ch == '/')
        return 63;

    throw std::runtime_error{"Invalid Base64 character: '"+std::string{1, (char)ch}+"'"};
}

byteArray_t decode(const std::string& input, bool needsPadding/*=false*/)
{
    byteArray_t output;

    for (size_t i{}; i < std::ceil(input.size()/4.f); ++i)
    {
        Sequence seq{};
        if (i*4+3 < input.size() && input[i*4+3] != BASE64_PADDING_CHAR)
            seq.first = charToIndex(input[i*4+3]);
        if (i*4+2 < input.size() && input[i*4+2] != BASE64_PADDING_CHAR)
            seq.second = charToIndex(input[i*4+2]);
        seq.third  = charToIndex(input[i*4 + 1]);
        seq.fourth = charToIndex(input[i*4 + 0]);

        uint8_t val;
        memcpy(&val, (uint8_t*)(&seq)+2, 1);
        output.push_back(val);

        if (needsPadding && i*4+3 >= input.size())
            throw std::runtime_error{"Input is not padded properly."};

        memcpy(&val, (uint8_t*)(&seq)+1, 1);
        if (val != 0 || (i*4+3 < input.size() && input[i*4+3] != BASE64_PADDING_CHAR))
            output.push_back(val);

        memcpy(&val, (uint8_t*)(&seq)+0, 1);
        if (val != 0 || (i*4+3 < input.size() && input[i*4+3] != BASE64_PADDING_CHAR))
            output.push_back(val);
    }

    return output;
}

} // namespace Base64
