#pragma once

#include <QAbstractItemModel>
#include <QDir>
#include <QQmlApplicationEngine>

#include <memory>

#include <files_processor.h>

namespace ui
{

class Gui
{
public:
    explicit Gui(const std::shared_ptr<barch::FilesProcessor> &filesProcessor, const QDir &workingDirectory);

    void show();

private:
    QQmlApplicationEngine m_engine;

    std::shared_ptr<barch::FilesProcessor> m_filesProcessor;
    std::unique_ptr<QAbstractItemModel> m_filesModel;
};

}
