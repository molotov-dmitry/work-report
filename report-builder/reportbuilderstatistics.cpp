#include "reportbuilderstatistics.h"

#include <QMap>
#include <QSet>
#include <QTextStream>

#define COL_RED    "\033[31m"
#define COL_GREEN  "\033[32m"
#define COL_YELLOW "\033[33m"

#define COL_RESET  "\033[0m"

struct StatisticsType
{
    int work       = 0;
    int illness    = 0;
    int vacation   = 0;
};

void printHours(QTextStream& stream, int plan, int actual, int width, int level, const QString& label)
{
    const char* color = "";

    if (actual == plan)
    {
        color = COL_GREEN;
    }
    else
    {
        if (level == 0)
        {
            color = COL_RED;
        }
        else
        {
            color = COL_YELLOW;
        }
    }

    QString labelInner(label + ": ");
    while (level--)
    {
        labelInner = labelInner.prepend("  ");
    }

    stream << labelInner.leftJustified(width + 2) <<
              color << actual << QString::fromUtf8(" ч.");

    if (actual != plan)
    {
        stream << " / " << plan << QString::fromUtf8(" ч.");
    }

    stream << COL_RESET << endl;
}

void printStatistics(const QList<ReportEntry>& plan,
                     const QList<ReportEntry>& actual)
{
    QTextStream stream(stdout);

    QSet<QString> workers;
    QMap<QString, StatisticsType> statPlan;
    QMap<QString, StatisticsType> statActual;

    foreach (const ReportEntry& entry, actual)
    {
        workers.insert(entry.name);

        switch (entry.type)
        {
        case TASK_ACTION:
            statActual[entry.name].work += entry.hours;
            break;

        case TASK_ILLNESS:
            statActual[entry.name].illness += entry.hours;
            break;

        case TASK_VACATION:
            statActual[entry.name].vacation += entry.hours;
            break;

        case TASK_COUNT:
            break;
        }
    }

    foreach (const ReportEntry& entry, plan)
    {
        workers.insert(entry.name);

        switch (entry.type)
        {
        case TASK_ACTION:
            statPlan[entry.name].work += entry.hours;
            break;

        case TASK_ILLNESS:
            statPlan[entry.name].illness += entry.hours;
            break;

        case TASK_VACATION:
            statPlan[entry.name].vacation += entry.hours;
            break;

        case TASK_COUNT:
            break;
        }
    }

    int width = 0;

    foreach (const QString& dev, workers.values())
    {
        if (dev.length() > width)
        {
            width = dev.length();
        }
    }

    QStringList workersList = workers.values();
    workersList.sort();

    foreach (const QString& dev, workersList)
    {
        StatisticsType planHours = statPlan.value(dev);
        StatisticsType actualHours = statActual.value(dev);

        int totalPlan = planHours.work + planHours.vacation + planHours.illness;
        int totalActual = actualHours.work + actualHours.vacation + actualHours.illness;

        printHours(stream, totalPlan,          totalActual,          width, 0, dev);
        printHours(stream, planHours.work,     actualHours.work,     width, 1, "Работа");
        printHours(stream, planHours.vacation, actualHours.vacation, width, 1, "Отпуск/отгулы");
        printHours(stream, planHours.illness,  actualHours.illness,  width, 1, "Болезнь");

        stream << endl;
    }
}
