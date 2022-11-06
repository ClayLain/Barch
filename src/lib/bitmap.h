#pragma once

#include <vector>

#include "byte_utils.h"

namespace barch
{

struct BitmapView
{
    int width = 0;
    int height = 0;
    const Byte * bytes = nullptr;
};

struct Bitmap
{
    int width = 0;
    int height = 0;
    std::vector<Byte> bytes;
};

}
