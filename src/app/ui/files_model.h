#pragma once

#include <QAbstractListModel>

#include <QDir>
#include <QString>
#include <QVector>
#include <QFileInfo>

namespace ui
{

class FilesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        FileName = Qt::DisplayRole,
        SizeDescription = Qt::UserRole,
        FilePath
    };

    explicit FilesModel(
            const QDir &folder,
            const QVector<QString> &filters,
            QObject *parent = nullptr
            );

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void errorPopupRequested(const QString &message);

public slots:
    void onProcessingRequested(const QString &path);

private:
    QString getFileName(const QModelIndex &index) const;
    QString getFileSize(const QModelIndex &index) const;
    QString getFilePath(const QModelIndex &index) const;

    void updateFileList();

    QDir m_folder;
    QVector<QString> m_filters;
    QVector<QFileInfo> m_currentFileList;
};

}
