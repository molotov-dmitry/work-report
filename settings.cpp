#include "settings.h"

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

    mailTo = configObject["mailTo"].toString();

    if (configObject.contains("dataPath"))
    {
        workPath = configObject["dataPath"].toString();
    }

    mUserFirstName = configObject["userFirstName"].toString();
    mUserSurname   = configObject["userSurname"].toString();
    mUserLastName  = configObject["userLastName"].toString();

    mDepartment    = configObject["department"].toString();

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

    configObject["mailTo"]   = mailTo;
    configObject["dataPath"] = workPath;

    configObject["userFirstName"] = mUserFirstName;
    configObject["userSurname"]   = mUserSurname;
    configObject["userLastName"]  = mUserLastName;

    configObject["department"]    = mDepartment;

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

QString Settings::getUserName(bool shortFormat) const
{
    QStringList name;

    if (not mUserSurname.isEmpty())
    {
        name.append(mUserSurname);
    }

    if (not mUserFirstName.isEmpty())
    {
        if (shortFormat)
        {
            name.append(mUserFirstName.left(1).toUpper() + ".");
        }
        else
        {
            name.append(mUserFirstName);
        }
    }

    if (not mUserLastName.isEmpty())
    {
        if (shortFormat)
        {
            name.append(mUserLastName.left(1).toUpper() + ".");
        }
        else
        {
            name.append(mUserLastName);
        }
    }

    return name.join(' ');
}

QString Settings::getUserFirstName() const
{
    return mUserFirstName;
}

void Settings::setUserFirstName(const QString& value)
{
    mUserFirstName = value;
}

QString Settings::getUserSurname() const
{
    return mUserSurname;
}

void Settings::setUserSurname(const QString& value)
{
    mUserSurname = value;
}

QString Settings::getUserLastName() const
{
    return mUserLastName;
}

void Settings::setUserLastName(const QString& value)
{
    mUserLastName = value;
}

QString Settings::getDepartment() const
{
    return mDepartment;
}

void Settings::setDepartment(const QString& value)
{
    mDepartment = value;
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
