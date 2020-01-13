#include "projecttemplates.h"

#include "settingsdir.h"

#include <QStandardPaths>
#include <QDir>
#include <QTextStream>

ProjectTemplates::ProjectTemplates()
{

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

bool ProjectTemplates::read(const Settings& settings)
{
    clear();

    QString templatesPath = getFilePath(settings);
    if (templatesPath.isEmpty())
    {
        return false;
    }

    if (read(templatesPath))
    {
        return true;
    }
    else
    {
        return (migrate(settings));
    }
}

bool ProjectTemplates::save(const Settings& settings)
{
    QString configPath = getFilePath(settings, true);
    if (configPath.isEmpty())
    {
        return false;
    }

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

QString ProjectTemplates::getFilePath(const Settings& settings, bool createDir)
{
    QString workPath = settings.getWorkPath();
    if (workPath.isEmpty())
    {
        workPath = SettingsDir::getSettingsDir(false);
    }

    QDir dir;

    if (not dir.cd(workPath))
    {
        if (createDir)
        {
            dir.mkpath(workPath);

            if (not dir.cd(workPath))
            {
                return QString();
            }
        }
        else
        {
            return QString();
        }
    }

    return dir.absoluteFilePath(".templates");
}

bool ProjectTemplates::read(const QString& filePath)
{
    QFile projectsFile(filePath);

    if (not projectsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
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

    return true;
}

bool ProjectTemplates::migrate(const Settings& settings)
{
    QDir dir(SettingsDir::getSettingsDir(false));

    if (not dir.exists())
    {
        return false;
    }

    QString configPath = dir.absoluteFilePath("projects");

    if (read(configPath))
    {
        if (save(settings))
        {
            QFile::remove(configPath);
            return true;
        }
    }

    return false;
}
