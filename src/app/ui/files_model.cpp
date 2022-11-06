#include "files_model.h"

namespace ui
{

FilesModel::FilesModel(
        const QDir &folder,
        const QVector<QString> &filters,
        const std::shared_ptr<barch::FilesProcessor> &processor,
        QObject *parent
        )
    : QAbstractListModel(parent)
    , m_folder { folder }
    , m_filters { filters }
    , m_currentFileList { m_folder.entryInfoList(m_filters, QDir::Files | QDir::NoDotAndDotDot) }
    , m_processor { processor }
{
    QObject::connect(
                m_processor.get(),
                &barch::FilesProcessor::operationStarted,
                this,
                &FilesModel::onOperationStarted
                );
    QObject::connect(
                m_processor.get(),
                &barch::FilesProcessor::operationFinished,
                this,
                &FilesModel::onOperationFinished
                );
    QObject::connect(
                m_processor.get(),
                &barch::FilesProcessor::operationFailed,
                this,
                &FilesModel::onOperationFailed
                );
}

int FilesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_currentFileList.count();
}

QVariant FilesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        Q_ASSERT(false);
        return {};
    }

    switch (role) {
    case Roles::FileName: return getFileName(index);
    case Roles::SizeDescription: return getFileSize(index);
    case Roles::FilePath: return getFilePath(index);
    default: {
        Q_ASSERT(false);
        return {};
    }
    }
}

QHash<int, QByteArray> FilesModel::roleNames() const
{
    return {
        { Roles::FileName, "fileName" },
        { Roles::SizeDescription, "sizeDescription" },
        { Roles::FilePath, "filePath" }
    };
}

QString FilesModel::getFileName(const QModelIndex &index) const
{
    const auto originalName = m_currentFileList.at(index.row()).fileName();
    const auto path = getFilePath(index);
    const auto operation = m_curentFileOperations.value(path, barch::OperationType::Invalid);
    switch (operation) {
    case barch::OperationType::Decode: return originalName + " " + "(decoding)";
    case barch::OperationType::Encode: return originalName + " " + "(encoding)";
    default: return originalName;
    }
}

QString FilesModel::getFileSize(const QModelIndex &index) const
{
    const auto locale = QLocale();
    const auto &fileInfo = m_currentFileList.at(index.row());
    return locale.formattedDataSize(fileInfo.size());
}

QString FilesModel::getFilePath(const QModelIndex &index) const
{
    return m_currentFileList.at(index.row()).absoluteFilePath();
}

QModelIndex FilesModel::indexOf(const QString &filePath) const
{
    const auto samePath = [&filePath](const QFileInfo &info){
        return info.absoluteFilePath() == filePath;
    };

    const auto it = std::find_if(m_currentFileList.cbegin(), m_currentFileList.cend(), samePath);
    if (it == m_currentFileList.cend()) {
        return {};
    }

    return index(std::distance(m_currentFileList.cbegin(), it));
}

void FilesModel::updateFileList()
{
    beginResetModel();
    m_currentFileList = m_folder.entryInfoList(m_filters, QDir::Files | QDir::NoDotAndDotDot);
    endResetModel();
}

void FilesModel::onProcessingRequested(const QString &path)
{
    m_processor->process(path);
}

void FilesModel::onOperationStarted(const QString &filePath, barch::OperationType opType)
{
    m_curentFileOperations[filePath] = opType;
    const auto i = indexOf(filePath);
    emit dataChanged(i, i, { Roles::FileName });
}

void FilesModel::onOperationFinished(const QString &filePath)
{
    m_curentFileOperations.remove(filePath);
    updateFileList();
}

void FilesModel::onOperationFailed(const QString &filePath, const QString &errorDescription)
{
    m_curentFileOperations.remove(filePath);
    emit errorPopupRequested(errorDescription);
}

}
