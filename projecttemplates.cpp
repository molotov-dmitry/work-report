#include "projecttemplates.h"

#include <QStandardPaths>
#include <QDir>
#include <QTextStream>

ProjectTemplates::ProjectTemplates()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    QDir dir(path);

    if (not dir.cd("work-report"))
    {
        dir.mkdir("work-report");
        dir.cd("work-report");
    }

    if (not dir.exists())
    {
        return;
    }

    QString configPath = dir.absoluteFilePath("projects");

    QFile projectsFile(configPath);

    if (not projectsFile.open(QIODevice::ReadOnly))
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
