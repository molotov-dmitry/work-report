#include "plannedtasks.h"

#include <QDir>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "settings.h"
#include "values.h"

PlannedTasks::PlannedTasks(const Settings& settings) : mSettings(settings)
{

}

PlannedTasks::PlannedTasks(const Settings& settings, const QDate& date) :
    mSettings(settings)
{
    load(date);
}

PlannedTasks::PlannedTasks(const PlannedTasks& other) :
    mPlannedTasks(other.mPlannedTasks), mSettings(other.mSettings)
{

}

void PlannedTasks::load(const QDate& date)
{
    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        return;
    }

    if (not taskDir.cd(".plan"))
    {
        return;
    }

    QString fileName = date.toString("yyyy-MM") +".json";

    QString taskPath(taskDir.absoluteFilePath(fileName));

    //// Open file =============================================================

    QFile file(taskPath);

    if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray taskByteArray = file.readAll();

    //// Load JSON =============================================================

    QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

    QJsonObject reportObject = taskDocument.object();

    //// Add tasks =============================================================

    foreach (const QJsonValue& value, reportObject["tasks"].toArray())
    {
        QJsonObject object = value.toObject();

        //// Check type --------------------------------------------------------

        QByteArray typeStr = object["type"].toString().toUtf8();

        if (typeStr != QByteArray(gValuesTaskTypes[TASK_ACTION].jsonValue))
        {
            continue;
        }

        //// Get data ----------------------------------------------------------

        PlannedTask task;
        task.hours       = object["hours"].toInt();
        task.description = object["description"].toString();

        QString project  = object["project"].toString();
        QString product  = object["product"].toString();

        //// Get action --------------------------------------------------------

        QByteArray actionStr = object["action"].toString().toUtf8();

        for (int i = 0; i < ACTION_COUNT; ++i)
        {
            if (QByteArray(gValuesActionTypes[i].jsonValue) == actionStr)
            {
                task.action = i;
            }
        }

        //// Add planned task --------------------------------------------------

        QPair<QString, QString> key(project, product);

        mPlannedTasks[key].append(task);
    }

    //// =======================================================================
}

QMap< QPair<QString, QString>, QList<PlannedTask> > PlannedTasks::getPlannedTasks() const
{
    return mPlannedTasks;
}
