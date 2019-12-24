#ifndef SETINGSDIR_H
#define SETINGSDIR_H

#include <QString>

class SettingsDir
{
public:
    static QString getSettingsDir(bool create);

private:
    SettingsDir();
    SettingsDir(const SettingsDir& other);
};

#endif // SETINGSDIR_H
