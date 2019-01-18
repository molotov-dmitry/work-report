#ifndef PROJECTTEMPLATES_H
#define PROJECTTEMPLATES_H

#include <QMap>
#include <QStringList>

class ProjectTemplates
{
public:
    ProjectTemplates();

    const QStringList& getProjects() const;
    const QMap<QString, QStringList>& getData() const;

private:
    QStringList mProjects;
    QMap<QString, QStringList> mData;

};

#endif // PROJECTTEMPLATES_H
