#include "gui.h"

#include <QQmlContext>

#include <files_processor.h>

#include "files_model.h"

namespace ui
{

QVector<QString> supportedFilesFilter()
{
    return {
        "*.bmp",
        "*.png",
        "*.barch"
    };
}

Gui::Gui(const std::shared_ptr<barch::FilesProcessor> &filesProcessor, const QDir &workingDirectory)
    : m_filesProcessor { filesProcessor }
    , m_filesModel { std::make_unique<FilesModel>(workingDirectory, supportedFilesFilter(), m_filesProcessor) }
{
}

void Gui::show()
{
    m_engine.rootContext()->setContextProperty("barch_model", m_filesModel.get());
    m_engine.load(QUrl { "qrc:/qmlmodule/qml/MainWindow.qml" });
}

}
