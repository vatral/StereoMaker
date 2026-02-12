#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDir>


static void setupLoggingIni()
{
    // App-specific config directory (uses org/app name, varies by platform)
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    const QString iniPath = dir + "/qtlogging.ini";

    QDir().mkpath(QFileInfo(iniPath).absolutePath());

    // Must be set before Qt's logging rules are initialized/used
    qputenv("QT_LOGGING_CONF", QFile::encodeName(iniPath));
}


int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("StereoMaker");
    QCoreApplication::setOrganizationName("Vadim Troshchinskiy");
    QCoreApplication::setOrganizationDomain("troshchinskiy.com");

    setupLoggingIni();

    QApplication a(argc, argv);

    a.setApplicationDisplayName("Stereo Maker");


    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "StereoMaker_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
