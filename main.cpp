#include <iostream>
#include <stdint.h>
#include <vector>
#include <cmath>
#include <cassert>
#include <cstring>
#include <string_view>

#define PADDING_CHAR '='

using byteArray_t = std::vector<uint8_t>;

struct Sequence
{
    uint first  : 6;
    uint second : 6;
    uint third  : 6;
    uint fourth : 6;
} __attribute__((packed));

uint8_t indexToChar(uint index)
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

std::string toBase64(const byteArray_t& input)
{
    std::string output;

    //const size_t outCount = std::ceil(input.size()/3.f*4.f);
    //std::cout << "Output: " << outCount << " characters\n";

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
        output += (seq.second == 0 && i == end-1) ? PADDING_CHAR : indexToChar(seq.second);
        output += (seq.first == 0 && i == end-1) ? PADDING_CHAR : indexToChar(seq.first);
    }

    return output;
}

static bool checkEncoding(const std::string& input, const std::string& expected)
{
    byteArray_t input_;
    input_.reserve(input.size());
    for (char val : input)
        input_.push_back(val);
    const std::string decoded = toBase64(input_);
    std::cout << "Decoded:  '" << input << "'\nResult:   '" << decoded << "'\nExpected: '" << expected << "'\n";

    const bool ok = (decoded == expected);
    std::cout << (ok ? "\033[92mOK\033[0m" : "\033[91mFAIL\033[0m") << '\n';
    return ok;
}

static constexpr std::string_view testTable[][2] = {
        {"Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"},
        {"Many hands make light work",  "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcms="},
        {"Many hands make light wor",   "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcg=="},
        {"Many hands make light wo",    "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdv"},
        {"Many hands make light w",     "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHc="},
        {"aswuh2378asdfhSDF=(SDVS+E(=+!\"+!(=SVLCV?NX+\"Q=(!xyner82u3rfsdfj", "YXN3dWgyMzc4YXNkZmhTREY9KFNEVlMrRSg9KyEiKyEoPVNWTENWP05YKyJRPSgheHluZXI4MnUzcmZzZGZq"},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfj", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZq"},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfjg", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZqZw=="},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfjgg", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZqZ2c="},
        {" ", "IA=="}
};

int main()
{
    static constexpr size_t testCount = sizeof(testTable)/sizeof(testTable[0]);
    for (size_t i{}; i < testCount; ++i)
    {
        assert(checkEncoding(testTable[i][0].data(), testTable[i][1].data()));
    }
    
    //const std::string input = "Many hands make light work";
    //byteArray_t input_;
    //input_.reserve(input.size());
    //for (char val : input)
    //    input_.push_back(val);
    //const std::string decoded = toBase64(input_);
    //std::cout << decoded << '\n';

    return 0;
}
