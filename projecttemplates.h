#ifndef PROJECTTEMPLATES_H
#define PROJECTTEMPLATES_H

#include <QMap>
#include <QStringList>

#include "settings.h"

class ProjectTemplates
{
public:
    ProjectTemplates();

    const QStringList& getProjects() const;
    const QMap<QString, QStringList>& getData() const;

    void clear();
    void addProject(const QString& project);
    void addProduct(const QString& project, const QString& product);

    bool read(const Settings& settings);
    bool save(const Settings& settings);

private:
    QStringList mProjects;
    QMap<QString, QStringList> mData;

    QString getFilePath(const Settings& settings, bool createDir = false);

    bool read(const QString& filePath);

    bool migrate(const Settings& settings);

};

#endif // PROJECTTEMPLATES_H
