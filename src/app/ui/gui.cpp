#include "gui.h"

namespace ui
{

Gui::Gui(const QDir &workingDirectory)
{
}

void Gui::show()
{
    m_engine.load(QUrl { "qrc:/qmlmodule/qml/MainWindow.qml" });
}

}
