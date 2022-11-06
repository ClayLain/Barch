#include "archive.h"

#include <iterator>
#include <span>
#include <limits.h>

namespace barch
{

static const auto g_maxChunkSize = 4;

struct BitmapArchive
{
    int width = 0;
    int height = 0;
    std::vector<bool> emptyIndex;
    std::vector<bool> dataBits;
};

uint32_t chunksPerLine(const BitmapView &bitmap)
{
    const auto chunkCount = bitmap.width / g_maxChunkSize;
    const auto unalignedChunkSize = bitmap.width % g_maxChunkSize;
    return unalignedChunkSize ? chunkCount + 1 : chunkCount;
}

std::vector<Byte> serialize(const BitmapArchive &arch)
{
    const auto emptyIndexBytes = toBytes(arch.emptyIndex);
    const auto dataBytes = toBytes(arch.dataBits);
    const auto widthBytes = toBytes(arch.width);
    const auto heightBytes = toBytes(arch.height);
    const auto dataSizeBits = toBytes(arch.dataBits.size());

    std::vector<Byte> result;
    result.reserve(
                widthBytes.size() +
                heightBytes.size() +
                emptyIndexBytes.size() +
                dataSizeBits.size() +
                dataBytes.size()
                );

    result.insert(result.end(), widthBytes.begin(), widthBytes.end());
    result.insert(result.end(), heightBytes.begin(), heightBytes.end());
    result.insert(result.end(), emptyIndexBytes.begin(), emptyIndexBytes.end());
    result.insert(result.end(), dataSizeBits.begin(), dataSizeBits.end());
    result.insert(result.end(), dataBytes.begin(), dataBytes.end());

    return result;
}

void writeUncompressedData(BitmapArchive &archive, const BitmapView &bitmap, int chunkOffset, int chunkSize)
{
    archive.dataBits.push_back(1);
    archive.dataBits.push_back(1);
    const auto span = std::span<const Byte>(&bitmap.bytes[chunkOffset], chunkSize);
    const auto bits = fromBytes(span, span.size_bytes() * CHAR_BIT);
    archive.dataBits.insert(archive.dataBits.end(), bits.begin(), bits.end());
}

std::vector<Byte> archive(const BitmapView &bitmap)
{
    if (bitmap.height == 0 || bitmap.width == 0) {
        return {};
    }

    BitmapArchive archive;
    archive.width = bitmap.width;
    archive.height = bitmap.height;

    const auto chunkCount = chunksPerLine(bitmap);
    for (int line = 0; line < bitmap.height; ++line) {
        const auto lineOffset = bitmap.width * line;
        bool nonWhitePixelFound = false;
        for (int byteOffset = 0; byteOffset < archive.width; ++byteOffset) {
            if (bitmap.bytes[lineOffset + byteOffset] != 0xff) {
                nonWhitePixelFound = true;
                break;
            }
        }

        if (!nonWhitePixelFound) {
            archive.emptyIndex.push_back(true);
            continue;
        }

        archive.emptyIndex.push_back(false);
        for (int chunk = 0; chunk < chunkCount; ++chunk) {
            const auto chunkOffset = lineOffset + (chunk * g_maxChunkSize);
            const auto chunkEnd = std::min(chunkOffset + g_maxChunkSize, lineOffset + archive.width);
            const auto chunkSize = chunkEnd - chunkOffset;
            if (chunkSize != g_maxChunkSize) {
                writeUncompressedData(archive, bitmap, chunkOffset, chunkSize);
                continue;
            }

            const bool isWhiteSequenceStart = bitmap.bytes[chunkOffset] == 0xff;
            const bool isBlackSequenceStart = bitmap.bytes[chunkOffset] == 0x00;
            if (!isWhiteSequenceStart && !isBlackSequenceStart) {
                writeUncompressedData(archive, bitmap, chunkOffset, chunkSize);
                continue;
            }

            bool mixedBytesFound = false;
            for (int i = 1; i < g_maxChunkSize; ++i) {
                const auto chunkByte = bitmap.bytes[chunkOffset + i];
                if (isWhiteSequenceStart && chunkByte != 0xff || isBlackSequenceStart && chunkByte != 0x00) {
                    writeUncompressedData(archive, bitmap, chunkOffset, chunkSize);
                    mixedBytesFound = true;
                    break;
                }
            }

            if (mixedBytesFound) {
                continue;
            }

            if (isWhiteSequenceStart) {
                archive.dataBits.push_back(0);
            } else {
                archive.dataBits.push_back(1);
                archive.dataBits.push_back(0);
            }
        }
    }

    return serialize(archive);
}

Bitmap unarchive(const std::vector<Byte> &bytes)
{
    Bitmap result;
    result.width = fromBytes(std::span<const Byte>(&bytes[0], sizeof(int)));
    result.height = fromBytes(std::span<const Byte>(&bytes[sizeof(int)], sizeof(int)));

    const auto emptyIndexBytesCount = bytesForBits(result.height);
    const auto emptyIndexOffset = sizeof(int) * 2;
    const auto emptyIndexBytes = std::span<const Byte>(&bytes[emptyIndexOffset], emptyIndexBytesCount);
    const auto emptyIndex = fromBytes(emptyIndexBytes, result.height);

    const auto imageDataSizeOffset = emptyIndexOffset + emptyIndexBytesCount;
    const auto dataSizeBits = fromBytes(std::span<const Byte>(&bytes[imageDataSizeOffset], sizeof(int)));
    const auto imageDataOffset = imageDataSizeOffset + sizeof(int);
    auto imageBits = std::vector<bool> {};
    if (dataSizeBits) {
        const auto imageData = std::span<const Byte>(&bytes[imageDataOffset], bytesForBits(dataSizeBits));
        imageBits = fromBytes(imageData, dataSizeBits);
    }

    const auto emptyLine = std::vector<Byte>(result.width, 0xff);
    static const auto whiteChunk = std::vector<Byte> { 0xff, 0xff, 0xff, 0xff };
    static const auto blackChunk = std::vector<Byte> { 0x00, 0x00, 0x00, 0x00 };

    auto currentDataBit = 0;
    for (int line = 0; line < result.height; ++line) {
        if (emptyIndex[line]) {
            result.bytes.insert(result.bytes.end(), emptyLine.begin(), emptyLine.end());
            continue;
        }

        std::vector<Byte> newLine;
        while (newLine.size() < result.width) {
            const int availableBitsInLine = (result.width - newLine.size()) * CHAR_BIT;
            if (imageBits[currentDataBit] == 0) {
                newLine.insert(newLine.end(), whiteChunk.begin(), whiteChunk.end());
                ++currentDataBit;
            } else if (imageBits[currentDataBit] == 1) {
                ++currentDataBit;
                if (imageBits[currentDataBit] == 0) {
                    ++currentDataBit;
                    newLine.insert(newLine.end(), blackChunk.begin(), blackChunk.end());
                } else {
                    ++currentDataBit;
                    const auto startChunkOffset = currentDataBit;
                    const auto endChunkOffset = std::min(
                                startChunkOffset + (CHAR_BIT * g_maxChunkSize),
                                startChunkOffset + availableBitsInLine
                                );
                    std::vector<bool> chunk(imageBits.begin() + startChunkOffset, imageBits.begin() + endChunkOffset);
                    const auto chunkBytes = toBytes(chunk);
                    newLine.insert(newLine.end(), chunkBytes.begin(), chunkBytes.end());
                    currentDataBit += chunk.size();
                }
            }
        }
        result.bytes.insert(result.bytes.end(), newLine.begin(), newLine.end());
    }

    return result;
}

}
