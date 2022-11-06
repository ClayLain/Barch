#include "files_model.h"

namespace ui
{

FilesModel::FilesModel(
        const QDir &folder,
        const QVector<QString> &filters,
        QObject *parent
        )
    : QAbstractListModel(parent)
    , m_folder { folder }
    , m_filters { filters }
    , m_currentFileList { m_folder.entryInfoList(m_filters, QDir::Files | QDir::NoDotAndDotDot) }
{
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
    return m_currentFileList.at(index.row()).fileName();
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

void FilesModel::updateFileList()
{
    beginResetModel();
    m_currentFileList = m_folder.entryInfoList(m_filters, QDir::Files | QDir::NoDotAndDotDot);
    endResetModel();
}

void FilesModel::onProcessingRequested(const QString &path)
{
}

}
