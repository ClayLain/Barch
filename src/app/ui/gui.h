#pragma once

#include <QAbstractItemModel>
#include <QDir>
#include <QQmlApplicationEngine>

namespace ui
{

class Gui
{
public:
    explicit Gui(const QDir &workingDirectory);

    void show();

private:
    QQmlApplicationEngine m_engine;
    std::unique_ptr<QAbstractItemModel> m_filesModel;
};

}
