#include "byte_utils.h"

namespace barch
{

int bytesForBits(int bitCount)
{
    return bitCount % CHAR_BIT
            ? (bitCount / CHAR_BIT) + 1
            : (bitCount / CHAR_BIT);
}

std::vector<Byte> toBytes(const std::vector<bool> &bits)
{
    const auto numBytes = bytesForBits(bits.size());

    std::vector<Byte> result;
    result.reserve(numBytes);

    int offset = 0;
    for (int byteIndex = 0; byteIndex < numBytes; ++byteIndex) {
        Byte nextByte = 0;
        for (int bitIndex = 0; bitIndex < CHAR_BIT; ++bitIndex) {
            if (offset >= bits.size()) {
                break;
            }

            const auto bit = bits[offset];
            nextByte |= (bit << bitIndex);
            ++offset;
        }

        result.push_back(nextByte);
    }

    return result;
}

std::vector<Byte> toBytes(int value)
{
    std::vector<Byte> result;
    result.reserve(sizeof(int));
    result.push_back(value >> 0 * CHAR_BIT);
    result.push_back(value >> 1 * CHAR_BIT);
    result.push_back(value >> 2 * CHAR_BIT);
    result.push_back(value >> 3 * CHAR_BIT);
    return result;
}

std::vector<bool> fromBytes(const std::span<const Byte> &bytes, int bitCount)
{
    std::vector<bool> result;
    result.reserve(bitCount);

    for (auto bitIndex = 0; bitIndex < bitCount; ++bitIndex) {
        const auto byteIndex = bitIndex / CHAR_BIT;
        const auto byte = bytes[byteIndex];
        const auto mask = 0x1 << bitIndex % CHAR_BIT;
        result.push_back((byte & mask) > 0);
    }

    return result;
}

int fromBytes(const std::span<const Byte> &bytes)
{
    return *reinterpret_cast<const int *>(bytes.data());
}

}
