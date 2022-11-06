#pragma once

#include <QObject>

namespace barch
{

enum class OperationType
{
    Invalid,
    Encode,
    Decode
};

class FilesProcessor : public QObject
{
    Q_OBJECT

public:
    explicit FilesProcessor(QObject *parent = nullptr);

    void process(const QString &filePath);

signals:
    void operationStarted(const QString &filePath, barch::OperationType opType);
    void operationFinished(const QString &filePath);
    void operationFailed(const QString &filePath, const QString &errorDescription);

private slots:
    void encodeFile(const QString &sourceFilePath, const QString &destFilePath);
    void decodeFile(const QString &sourceFilePath, const QString &destFilePath);
};

}
