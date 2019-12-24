﻿#include "settings.h"

#include "settingsdir.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>

Settings::Settings()
{
    loadDefaultSettings();

    QDir dir(SettingsDir::getSettingsDir(false));

    if (not dir.exists())
    {
        return;
    }

    QString configPath = dir.absoluteFilePath("config.json");

    QFile configFile(configPath);

    if (not configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QJsonDocument configDocument = QJsonDocument::fromJson(configFile.readAll());

    QJsonObject configObject = configDocument.object();

    userName = configObject["userName"].toString();
    mailTo   = configObject["mailTo"].toString();
    workPath = configObject["dataPath"].toString();

#ifdef Q_OS_WIN
    outlookPath = configObject["outlookPath"].toString();
#endif

    configFile.close();
}

void Settings::save()
{
    QDir dir(SettingsDir::getSettingsDir(true));

    if (not dir.exists())
    {
        return;
    }

    QString configPath = dir.absoluteFilePath("config.json");
    QString configPathBak = dir.absoluteFilePath("config.json.bak");

    QFile::remove(configPathBak);
    QFile::rename(configPath, configPathBak);

    QJsonObject configObject;

    configObject["userName"] = userName;
    configObject["mailTo"] = mailTo;
    configObject["dataPath"] = workPath;

#ifdef Q_OS_WIN
    configObject["outlookPath"] = outlookPath;
#endif

    QFile configFile(configPath);

    if (not configFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QJsonDocument configDocument(configObject);

    configFile.write(configDocument.toJson());

    configFile.close();
}

void Settings::loadDefaultSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QDir dir(path);

    workPath = dir.absoluteFilePath("Отчеты");
}

QString Settings::getUserName() const
{
    return userName;
}

void Settings::setUserName(const QString &value)
{
    userName = value;
}

QString Settings::getMailTo() const
{
    return mailTo;
}

void Settings::setMailTo(const QString &value)
{
    mailTo = value;
}

QString Settings::getWorkPath() const
{
    return workPath;
}

void Settings::setWorkPath(const QString &value)
{
    workPath = value;
}

#ifdef Q_OS_WIN
QString Settings::getOutlookPath() const
{
    return outlookPath;
}

void Settings::setOutlookPath(const QString &value)
{
    outlookPath = value;
}
#endif
