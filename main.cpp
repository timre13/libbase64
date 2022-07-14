#include <iostream>
#include <stdint.h>
#include <cassert>
#include <string_view>
#include "include/Base64.h"

static bool checkEncoding(const std::string& input, const std::string& expected, bool usePadding)
{
    std::cout << "Encoding:  '" << input << '\'';

    Base64::byteArray_t input_;
    input_.reserve(input.size());
    for (char val : input)
        input_.push_back(val);
    const std::string encoded = Base64::encode(input_, usePadding);

    std::cout << "\nResult:   '" << encoded << "'\nExpected: '" << expected << "'\n";

    const bool ok = (encoded == expected);
    std::cout << (ok ? "\033[92mOK\033[0m" : "\033[91mFAIL\033[0m") << '\n';
    return ok;
}

static bool checkDecoding(const std::string& input, const std::string& expected, bool needsPadding)
{
    std::cout << "Decoding:  '" << input << '\'';

    const Base64::byteArray_t output = Base64::decode(input, needsPadding);
    std::string output_;
    for (uint8_t val : output)
        output_.push_back(val);

    std::cout << "\nResult:   '" << output_ << "'\nExpected: '" << expected << "'\n";

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

struct TestCase
{
    std::string_view original;
    std::string_view encoded;
    bool hasMissingPadding;
};
static constexpr TestCase notPaddedTestTable[] = {
        {"Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu", false},
        {"Many hands make light work",  "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcms", true},
        {"Many hands make light wor",   "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcg", true},
        {"Many hands make light wo",    "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdv", false},
        {"Many hands make light w",     "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHc", true},
        {"aswuh2378asdfhSDF=(SDVS+E(=+!\"+!(=SVLCV?NX+\"Q=(!xyner82u3rfsdfj", "YXN3dWgyMzc4YXNkZmhTREY9KFNEVlMrRSg9KyEiKyEoPVNWTENWP05YKyJRPSgheHluZXI4MnUzcmZzZGZq", false},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfj", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZq", false},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfjg", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZqZw", true},
        {"asw3478asdfhSDF=(SDFKLJVS+cvkjnwer827u3rfsdfjgg", "YXN3MzQ3OGFzZGZoU0RGPShTREZLTEpWUytjdmtqbndlcjgyN3UzcmZzZGZqZ2c", true},
        {" ", "IA", true},
};

int main()
{
#if 1
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
            assert(checkDecoding(paddedTestTable[i][1].data(), paddedTestTable[i][0].data(), true));
        }
    }

    { // Test without padding
        std::cout << std::string(27, '=') << " Testing with no padding " << std::string(28, '=') << '\n';

        static constexpr size_t notPaddedTestCount = sizeof(notPaddedTestTable)/sizeof(notPaddedTestTable[0]);
        std::cout << std::string(35, '-') << " Encoding " << std::string(35, '-') << '\n';
        for (size_t i{}; i < notPaddedTestCount; ++i)
        {
            assert(checkEncoding(notPaddedTestTable[i].original.data(), notPaddedTestTable[i].encoded.data(), false));
        }

        std::cout << std::string(35, '-') << " Decoding " << std::string(35, '-') << '\n';
        for (size_t i{}; i < notPaddedTestCount; ++i)
        {
            const bool shouldThrow = notPaddedTestTable[i].hasMissingPadding;
            bool didThrow = false;
            try
            {
                assert(checkDecoding(notPaddedTestTable[i].encoded.data(), notPaddedTestTable[i].original.data(), true));
            }
            catch (const std::exception& e)
            {
                std::cout << "\nException thrown: " << e.what() << '\n';
                didThrow = true;
            }
            assert(shouldThrow == didThrow);
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
