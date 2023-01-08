#pragma once

extern "C"
{
    #include "Utils/lzma/pavlov/LzmaUtil.h"
}
#include <vector>

namespace LZMA
{
    // both of these are highly non-threadsafe, if two of these calls (even different ones) run at the same time it *will* mess things up.
    bool lzmaDecompress(const std::vector<unsigned char> &in, std::vector<unsigned char> &out);
    bool lzmaCompress(const std::vector<unsigned char> &in, std::vector<unsigned char> &out);
}