#pragma once

#include <vector>
#include <span>

namespace barch
{

using Byte = unsigned char;

int bytesForBits(int bitCount);
std::vector<Byte> toBytes(const std::vector<bool> &bits);
std::vector<Byte> toBytes(int value);
std::vector<bool> fromBytes(const std::span<const Byte> &bytes, int bitCount);
int fromBytes(const std::span<const Byte> &bytes);

}
