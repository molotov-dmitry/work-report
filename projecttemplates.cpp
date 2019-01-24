#include "projecttemplates.h"

#include "settingsdir.h"

#include <QStandardPaths>
#include <QDir>
#include <QTextStream>

ProjectTemplates::ProjectTemplates()
{
    QDir dir(SettingsDir::getSettingsDir(false));

    if (not dir.exists())
    {
        return;
    }

    QString configPath = dir.absoluteFilePath("projects");

    QFile projectsFile(configPath);

    if (not projectsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QString currentProject;

    QTextStream in(&projectsFile);

    while (not in.atEnd())
    {
        QString line = in.readLine();

        if (line.isEmpty())
        {
            continue;
        }

        if (line.startsWith(' '))
        {
            mData[currentProject].append(line.mid(1));
        }
        else
        {
            currentProject = line;
            mProjects.append(currentProject);
            mData.insert(currentProject, QStringList());
        }
    }

    projectsFile.close();
}

const QStringList &ProjectTemplates::getProjects() const
{
    return mProjects;
}

const QMap<QString, QStringList> &ProjectTemplates::getData() const
{
    return mData;
}

void ProjectTemplates::clear()
{
    mData.clear();
    mProjects.clear();
}

void ProjectTemplates::addProject(const QString &project)
{
    mProjects.append(project);
}

void ProjectTemplates::addProduct(const QString& project, const QString& product)
{
    mData[project].append(product);
}

bool ProjectTemplates::save()
{
    QDir dir(SettingsDir::getSettingsDir(true));

    if (not dir.exists())
    {
        return false;
    }

    QString configPath = dir.absoluteFilePath("projects");

    QFile projectsFile(configPath);

    if (not projectsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&projectsFile);

    foreach (const QString& project, mProjects)
    {
        out << project << endl;

        foreach (const QString& product, mData.value(project))
        {
            out << QString(" ") + product << endl;
        }
    }

    projectsFile.close();

    return true;
}
