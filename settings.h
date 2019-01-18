#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings
{
public:
    Settings();
    void save();

    void loadDefaultSettings();

    QString getUserName() const;
    void setUserName(const QString &value);

    QString getMailTo() const;
    void setMailTo(const QString &value);

    QString getWorkPath() const;
    void setWorkPath(const QString &value);

private:

    QString userName;
    QString mailTo;
    QString workPath;
};

#endif // SETTINGS_H
