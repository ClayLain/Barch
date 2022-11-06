#include "files_processor.h"

#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QtConcurrent>

#include <archive.h>

namespace barch
{

OperationType selectOperationType(const QString &path)
{
    static const auto opType = QHash<QString, OperationType> {
        { "bmp", OperationType::Encode },
        { "barch", OperationType::Decode },
    };

    const auto extension = QFileInfo { path }.suffix();
    const auto it = opType.find(extension);
    if (it == opType.end()) {
        return OperationType::Invalid;
    }

    return *it;
}

QString getDestFilePath(const QString &sourceFile, OperationType operation)
{
    const auto originalFileInfo = QFileInfo { sourceFile };
    const auto originalDir = originalFileInfo.absoluteDir();

    switch (operation) {
    case OperationType::Encode: {
        return originalDir.filePath(originalFileInfo.fileName() + "_packed.barch");
    }
    case OperationType::Decode: {
        return originalDir.filePath(originalFileInfo.fileName() + "_unpacked.bmp");
    }
    default: {
        Q_ASSERT(false);
        return {};
    }
    }
}

FilesProcessor::FilesProcessor(QObject *parent)
{
}

void FilesProcessor::process(const QString &filePath)
{
    const auto onOperationFinished = [this, filePath](){
        emit operationFinished(filePath);
    };

    const auto operation = selectOperationType(filePath);
    if (operation == barch::OperationType::Invalid) {
        emit operationFailed(filePath, "Unknown file");
        return;
    }

    const auto destFile = getDestFilePath(filePath, operation);

    emit operationStarted(filePath, operation);
    switch (operation) {
    case OperationType::Encode: {
        QtConcurrent::run(&FilesProcessor::encodeFile, this, filePath, destFile).then(this, onOperationFinished);
        break;
    }
    case OperationType::Decode: {
        QtConcurrent::run(&FilesProcessor::decodeFile, this, filePath, destFile).then(this, onOperationFinished);
        break;
    }
    default: return;
    }
}

void FilesProcessor::encodeFile(const QString &sourceFilePath, const QString &destFilePath)
{
    if (QFileInfo::exists(destFilePath)) {
        return;
    }

    auto file = QFile { destFilePath };
    if (!file.open(QFile::WriteOnly)) {
        return;
    }

    auto image = QImage { sourceFilePath };
    if (image.isNull()) {
        return;
    }

    if (!image.allGray()) {
        return;
    }

    if (!image.isGrayscale()) {
        image = image.convertToFormat(QImage::Format_Grayscale8);
    }

    const auto bitmap = BitmapView { image.width(), image.height(), image.bits() };
    const auto bytes = archive(bitmap);
    const auto byteArray = QByteArray(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    file.write(byteArray);
}

void FilesProcessor::decodeFile(const QString &sourceFilePath, const QString &destFilePath)
{
    if (QFileInfo::exists(destFilePath)) {
        return;
    }

    auto sourceFile = QFile { sourceFilePath };
    if (!sourceFile.open(QFile::ReadOnly)) {
        return;
    }

    const auto qtBytes = sourceFile.readAll();
    const auto bytes = std::vector<unsigned char>(qtBytes.begin(), qtBytes.end());
    const auto originalBytes = unarchive(bytes);
    auto image = QImage {
            originalBytes.bytes.data(),
            originalBytes.width,
            originalBytes.height,
            QImage::Format::Format_Grayscale8
        };
    image.save(destFilePath);
}

}
