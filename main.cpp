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
static_assert(sizeof(Sequence) == 3, "Struct 'Sequence' has invalid size. Unsupported platform or compiler.");

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

std::string toBase64(const byteArray_t& input, bool applyPadding=true)
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

        if (seq.second != 0 || i != end-1)
            output += indexToChar(seq.second);
        else if (applyPadding)
            output += PADDING_CHAR;

        if (seq.first != 0 || i != end-1)
            output += indexToChar(seq.first);
        else if (applyPadding)
            output += PADDING_CHAR;
    }

    return output;
}

uint charToIndex(uint8_t ch)
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

byteArray_t fromBase64(const std::string& input)
{
    //std::cout << "Decoding: '" << input << "'\n";

    byteArray_t output;

    for (size_t i{}; i < std::ceil(input.size()/4.f); ++i)
    {
        Sequence seq{};
        if (input[i*4+3] != PADDING_CHAR && i*4+3 < input.size())
            seq.first  = charToIndex(input[i*4 + 3]);
        if (input[i*4+2] != PADDING_CHAR && i*4+2 < input.size())
            seq.second = charToIndex(input[i*4 + 2]);
        seq.third  = charToIndex(input[i*4 + 1]);
        seq.fourth = charToIndex(input[i*4 + 0]);

        uint8_t val;
        memcpy(&val, (uint8_t*)(&seq)+2, 1);
        output.push_back(val);

        memcpy(&val, (uint8_t*)(&seq)+1, 1);
        if (val != 0 || (input[i*4+3] != PADDING_CHAR && i*4+3 < input.size()))
            output.push_back(val);
        memcpy(&val, (uint8_t*)(&seq)+0, 1);
        if (val != 0 || (input[i*4+3] != PADDING_CHAR && i*4+3 < input.size()))
            output.push_back(val);
    }

    return output;
}

static bool checkEncoding(const std::string& input, const std::string& expected, bool usePadding)
{
    byteArray_t input_;
    input_.reserve(input.size());
    for (char val : input)
        input_.push_back(val);
    const std::string decoded = toBase64(input_, usePadding);
    std::cout << "Encoded:  '" << input << "'\nResult:   '" << decoded << "'\nExpected: '" << expected << "'\n";

    const bool ok = (decoded == expected);
    std::cout << (ok ? "\033[92mOK\033[0m" : "\033[91mFAIL\033[0m") << '\n';
    return ok;
}

static bool checkDecoding(const std::string& input, const std::string& expected)
{
    const byteArray_t output = fromBase64(input);
    std::string output_;
    for (uint8_t val : output)
        output_.push_back(val);

    std::cout << "Decoded:  '" << input << "'\nResult:   '" << output_ << "'\nExpected: '" << expected << "'\n";

    const bool ok = (output_ == expected);
    std::cout << (ok ? "\033[92mOK\033[0m" : "\033[91mFAIL\033[0m") << '\n';
    return ok;
}

static constexpr std::string_view paddedTestTable[][2] = {
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

static constexpr std::string_view notPaddedTestTable[][2] = {
        {"Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"},
        {"Many hands make light work",  "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcms"},
        {"Many hands make light wor",   "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcg"},
        {"Many hands make light wo",    "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdv"},
        {"Many hands make light w",     "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHc"},
        {"aswuh2378asdfhSDF=(SDVS+E(=+!\"+!(=SVLCV?NX+\"Q=(!xyner82u3rfsdfj", "YXN3dWgyMzc4YXNkZmhTREY9KFNEVlMrRSg9KyEiKyEoPVNWTENWP05YKyJRPSgheHluZXI4MnUzcmZzZGZq"},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfj", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZq"},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfjg", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZqZw"},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfjgg", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZqZ2c"},
        {" ", "IA"}
};

int main()
{
#if 0
    std::cout << '\n';

    { // Test with padding
        std::cout << std::string(29, '=') << " Testing with padding " << std::string(29, '=') << '\n';

        static constexpr size_t paddedTestCount = sizeof(paddedTestTable)/sizeof(paddedTestTable[0]);
        std::cout << std::string(35, '-') << " Encoding " << std::string(35, '-') << '\n';
        for (size_t i{}; i < paddedTestCount; ++i)
        {
            assert(checkEncoding(paddedTestTable[i][0].data(), paddedTestTable[i][1].data(), true));
        }

        std::cout << std::string(35, '-') << " Decoding " << std::string(35, '-') << '\n';
        for (size_t i{}; i < paddedTestCount; ++i)
        {
            assert(checkDecoding(paddedTestTable[i][1].data(), paddedTestTable[i][0].data()));
        }
    }

    { // Test without padding
        std::cout << std::string(27, '=') << " Testing with no padding " << std::string(28, '=') << '\n';

        static constexpr size_t notPaddedTestCount = sizeof(notPaddedTestTable)/sizeof(notPaddedTestTable[0]);
        std::cout << std::string(35, '-') << " Encoding " << std::string(35, '-') << '\n';
        for (size_t i{}; i < notPaddedTestCount; ++i)
        {
            assert(checkEncoding(notPaddedTestTable[i][0].data(), notPaddedTestTable[i][1].data(), false));
        }

        std::cout << std::string(35, '-') << " Decoding " << std::string(35, '-') << '\n';
        for (size_t i{}; i < notPaddedTestCount; ++i)
        {
            assert(checkDecoding(notPaddedTestTable[i][1].data(), notPaddedTestTable[i][0].data()));
        }
    }

    std::cout << "ALL TESTS PASSED" << '\n';
#else
    (void)checkDecoding;
    (void)checkEncoding;
#endif
    
    //const std::string input = "Many hands make light work";
    //byteArray_t input_;
    //input_.reserve(input.size());
    //for (char val : input)
    //    input_.push_back(val);
    //const std::string decoded = toBase64(input_);
    //std::cout << decoded << '\n';


    //const byteArray_t output = fromBase64("TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcms");
    //std::string output_;
    //for (uint8_t val : output)
    //    output_.push_back(val);
    //std::cout << "Output: '" << output_ << "'\n";

    return 0;
}
