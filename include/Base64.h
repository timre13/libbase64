#pragma once

#include <string>
#include <stdint.h>
#include <vector>

#define BASE64_PADDING_CHAR '='

namespace Base64
{

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
byteArray_t decode(const std::string& input);

} // namespace Base64
