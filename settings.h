#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings
{
public:
    Settings();
    void save();

    void loadDefaultSettings();

    QString getUserName(bool shortFormat = true) const;

    QString getUserFirstName() const;
    void setUserFirstName(const QString &value);

    QString getUserSurname() const;
    void setUserSurname(const QString &value);

    QString getUserLastName() const;
    void setUserLastName(const QString &value);

    QString getDepartment() const;
    void setDepartment(const QString &value);

    QString getMailTo() const;
    void setMailTo(const QString &value);

    QString getWorkPath() const;
    void setWorkPath(const QString &value);

#ifdef Q_OS_WIN
    QString getOutlookPath() const;
    void setOutlookPath(const QString &value);
#endif

    int getWorkHours() const;
    void setWorkHours(int workHours);

private:

    QString mUserFirstName;
    QString mUserSurname;
    QString mUserLastName;

    QString mDepartment;

    QString mailTo;
    QString workPath;

    int     mWorkHours;

#ifdef Q_OS_WIN
    QString outlookPath;
#endif
};

#endif // SETTINGS_H
