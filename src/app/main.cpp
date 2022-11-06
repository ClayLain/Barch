#include <QGuiApplication>
#include <QCommandLineParser>

#include <ui/gui.h>

struct AppOptions
{
    QString workingFolder;
};

AppOptions parseOptions(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Barch");

    const auto defaultFolder = QCoreApplication::applicationDirPath();
    const auto folderOption = QCommandLineOption { { "f", "folder" }, "Working folder", "folder", defaultFolder };

    parser.addHelpOption();
    parser.addOption(folderOption);

    auto ok = parser.parse(args);
    if (ok) {
        return { parser.value(folderOption) };
    } else {
        return { defaultFolder };
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);

    const auto options = parseOptions(app.arguments());
    auto gui = ui::Gui { options.workingFolder };
    gui.show();

    return app.exec();
}
