#include "plannedtasks.h"

#include <QDir>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

#include "settings.h"
#include "values.h"

#include "common/reportimport.h"

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

    //// Load unplanned tasks ==================================================

    for (int i = -1; i <= 0; ++i)
    {
        QDate dateTask = date.addMonths(i);

        QDir dirMonth;

        if (not dirMonth.cd(mSettings.getWorkPath()))
        {
            // Warning
            continue;
        }

        QString dirName = dateTask.toString("yyyy-MM");

        if (not dirMonth.cd(dirName))
        {
            // Warning
            continue;
        }

        QStringList filters;
        filters << "Отчет ??.??.???? - ??.??.????.csv";

        foreach (const QFileInfo& d, dirMonth.entryInfoList(filters, QDir::Files))
        {
            ReportImport importer;
            QList<ReportEntry> entries;

            if (not importer.readReport(d.absoluteFilePath(), entries))
            {
                QString error = importer.lastError();

                if (importer.lastErrorLine() > -1)
                {
                    error.append(QString::asprintf("\nat line %d",
                                                   importer.lastErrorLine()));
                }

                QMessageBox::critical(nullptr,
                                      QString::fromUtf8("Импорт"),
                                      error);

                continue;
            }

            foreach (const ReportEntry& report, entries)
            {
                if (report.type != TASK_ACTION)
                {
                    continue;
                }

                QPair<QString, QString> key(report.project, report.product);

                bool contains = false;

                if (mPlannedTasks.contains(key))
                {
                    foreach(const PlannedTask& task, mPlannedTasks.value(key))
                    {
                        if (task.description == report.plan)
                        {
                            contains = true;
                            break;
                        }
                    }
                }

                if (not contains)
                {
                    PlannedTask task;
                    task.action      = report.action;
                    task.hours       = 0;
                    task.description = report.plan;

                    mPlannedTasks[key].append(task);
                }
            }
        }
    }

    //// Load unplanned unexported tasks =======================================

    {
        QDir taskDir;

        if (not taskDir.cd(mSettings.getWorkPath()))
        {
            // Warning
            return;
        }

        QString taskPath(taskDir.absoluteFilePath(".task.json"));

        //// Open file =========================================================

        QFile file(taskPath);

        if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return;
        }

        QByteArray taskByteArray = file.readAll();

        //// Load JSON =========================================================

        QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

        QJsonObject reportObject = taskDocument.object();

        if (not reportObject["exported"].toBool())
        {
            foreach (const QJsonValue& value, reportObject["tasks"].toArray())
            {
                QJsonObject object = value.toObject();

                //// Get type --------------------------------------------------

                int typeValue = 0;
                QByteArray typeStr = object["type"].toString().toUtf8();

                for (int i = 0; i < TASK_COUNT; ++i)
                {
                    if (QByteArray(gValuesTaskTypes[i].jsonValue) == typeStr)
                    {
                        typeValue = gValuesTaskTypes[i].value;
                    }
                }

                //// Parse action ----------------------------------------------

                if (typeValue == TASK_ACTION)
                {
                    //// Get string values -------------------------------------

                    QString project = object["project"].toString();
                    QString product = object["product"].toString();
                    QString plan    = object["plan"].toString();

                    //// -------------------------------------------------------

                    QPair<QString, QString> key(project, product);

                    bool contains = false;

                    if (mPlannedTasks.contains(key))
                    {
                        foreach(const PlannedTask& task, mPlannedTasks.value(key))
                        {
                            if (task.description == plan)
                            {
                                contains = true;
                                break;
                            }
                        }
                    }

                    if (not contains)
                    {
                        PlannedTask task;
                        task.action      = typeValue;
                        task.hours       = 0;
                        task.description = plan;

                        mPlannedTasks[key].append(task);
                    }
                }
            }
        }
    }

    //// =======================================================================
}

QMap< QPair<QString, QString>, QList<PlannedTask> > PlannedTasks::getPlannedTasks() const
{
    return mPlannedTasks;
}
