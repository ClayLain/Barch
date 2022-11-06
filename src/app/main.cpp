#include <QGuiApplication>
#include <QCommandLineParser>

#include <memory>

#include <ui/gui.h>
#include <files_processor.h>

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
    const auto filesProcessor = std::make_shared<barch::FilesProcessor>();
    auto gui = ui::Gui { filesProcessor, options.workingFolder };
    gui.show();

    return app.exec();
}
