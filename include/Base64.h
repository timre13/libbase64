#pragma once

#include <string>
#include <stdint.h>
#include <vector>
#include <stdexcept>

#define BASE64_PADDING_CHAR '='

namespace Base64
{

class InvalidBase64CharException : public std::runtime_error
{
public:
    InvalidBase64CharException(const char* what)
        : std::runtime_error{what}
    {
    }

    InvalidBase64CharException(const std::string& what)
        : std::runtime_error{what}
    {
    }
};

class InvalidPaddingException : public std::runtime_error
{
public:
    InvalidPaddingException(const char* what)
        : std::runtime_error{what}
    {
    }

    InvalidPaddingException(const std::string& what)
        : std::runtime_error{what}
    {
    }
};

using byteArray_t = std::vector<uint8_t>;

struct Sequence
{
    uint first  : 6;
    uint second : 6;
    uint third  : 6;
    uint fourth : 6;
} __attribute__((packed));
static_assert(sizeof(Sequence) == 3, "Struct 'Sequence' has invalid size. Unsupported platform or compiler.");

std::string encode(const byteArray_t& input, bool applyPadding=true);
byteArray_t decode(const std::string& input, bool needsPadding=false);

} // namespace Base64
