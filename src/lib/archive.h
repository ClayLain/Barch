#pragma once

#include <vector>

#include "bitmap.h"

namespace barch
{

std::vector<Byte> archive(const BitmapView &bitmap);
Bitmap unarchive(const std::vector<Byte> &bytes);

}
