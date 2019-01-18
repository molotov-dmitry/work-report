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

    void clear();
    void addProject(const QString& project);
    void addProduct(const QString& project, const QString& product);

    bool save();

private:
    QStringList mProjects;
    QMap<QString, QStringList> mData;

};

#endif // PROJECTTEMPLATES_H
