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

#ifdef Q_OS_WIN
    QString getOutlookPath() const;
    void setOutlookPath(const QString &value);
#endif

private:

    QString userName;
    QString mailTo;
    QString workPath;

#ifdef Q_OS_WIN
    QString outlookPath;
#endif
};

#endif // SETTINGS_H
