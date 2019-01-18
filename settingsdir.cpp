#include "settingsdir.h"

#include <QDir>
#include <QStandardPaths>

QString SettingsDir::getSettingsDir(bool create)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    QDir dir(path);

    QString settingsPath;

#ifdef Q_OS_WIN
    settingsPath = dir.absolutePath();
#else
    settingsPath = dir.absoluteFilePath("work-report");
#endif

    if (create)
    {
        dir.mkpath(settingsPath);
    }

    return settingsPath;
}

SettingsDir::SettingsDir()
{

}

SettingsDir::SettingsDir(const SettingsDir &other)
{
    Q_UNUSED(other);
}
