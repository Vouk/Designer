#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStyleFactory>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QCommandLineParser>

#include <boost/dll/import.hpp>

#include "../Core/voukoderpro_api.h"
#include "../Core/Version.h"

#include "mainwindow.h"
#include "sceneselectdialog.h"

int main(int argc, char *argv[])
{
    // Set up the application
    QApplication app(argc, argv);
    app.setApplicationName("VoukoderPro Designer");
    app.setApplicationVersion(APP_VERSION);
    app.setStyle("Fusion");

    // Set up translations
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Designer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    // Define command line options
    QCommandLineParser parser;
    parser.setApplicationDescription(app.applicationName());
    parser.addHelpOption();
    parser.addVersionOption();

    // Open a scene on e.g. double click
    QCommandLineOption openSceneFile("open", "Open a .scene file on startup.");
    parser.addOption(openSceneFile);

    // Show the scene selection dialog
    QCommandLineOption selectScene("select", "Show the scene selection dialog on startup.");
    parser.addOption(selectScene);

    parser.addPositionalArgument("name", QCoreApplication::translate("main", "Source file to copy."));

    // Process the actual command line arguments given by the user
    parser.process(app);

    boost::function<pluginapi_create_t> factory;

    try
    {
        factory = VoukoderProCreateInstance();
    }
    catch (boost::system::system_error e)
    {
        QMessageBox::critical(nullptr, app.applicationName(), "Unable to load voukoderpro.dll! (1)\n\n" + QString::fromStdString(e.what()));
        return -1;
    }

    std::shared_ptr<VoukoderPro::IClient> vkdrPro = nullptr;
    try
    {
        vkdrPro = factory();
    }
    catch (boost::system::system_error e)
    {
        QMessageBox::critical(nullptr, app.applicationName(), "Unable to load voukoderpro.dll! (2)\n\n" + QString::fromStdString(e.what()));
        return -1;
    }

    int ret = 0;

    // Initialize VoukoderPro
    if ((ret = vkdrPro->init()) < 0)
    {
        QMessageBox::critical(nullptr, app.applicationName(), "VoukoderPro initialization failed!");
        return ret;
    }

    // Handle command line params
    QString name = "";
    const QStringList args = parser.positionalArguments();
    if (parser.isSet(selectScene))
    {
        SceneSelectDialog dialog(vkdrPro, args.length() > 0 ? args.at(0) : "");
        if (dialog.exec() == QDialog::Accepted)
        {
            QTextStream out(stdout);
            out << dialog.selectedScene();

            return 0;
        }

        return -1;
    }
    else if (parser.isSet(openSceneFile))
    {
        name = args.length() > 0 ? args.at(0) : "";
    }
    else if (args.length() > 0)
    {
        name = args.at(0);
    }

    // Create and show main window
    MainWindow w(vkdrPro, name);
    w.show();

    return app.exec();
}
