#include "reportbuilderstatistics.h"

#include <QMap>
#include <QTextStream>

struct StatisticsType
{
    int hoursWork       = 0;
    int hoursIllness    = 0;
    int hoursVacation   = 0;
};

void printStatistics(const QList<ReportEntry> &list)
{
    QTextStream stream(stdout);

    QMap<QString, StatisticsType> statType;

    foreach (const ReportEntry& entry, list)
    {
        switch (entry.type)
        {
        case TASK_ACTION:
            statType[entry.name].hoursWork += entry.hours;
            break;

        case TASK_ILLNESS:
            statType[entry.name].hoursIllness += entry.hours;
            break;

        case TASK_VACATION:
            statType[entry.name].hoursVacation += entry.hours;
            break;

        default:
            break;
        }
    }

    foreach (const QString& dev, statType.keys())
    {
        StatisticsType stat = statType.value(dev);

        stream << dev << ": " << stat.hoursWork + stat.hoursVacation + stat.hoursIllness << QString::fromUtf8(" ч.") << endl;

        stream << QString::fromUtf8("  Работа:        ") << stat.hoursWork << QString::fromUtf8(" ч.") << endl;
        stream << QString::fromUtf8("  Отпуск/отгулы: ") << stat.hoursVacation << QString::fromUtf8(" ч.") << endl;
        stream << QString::fromUtf8("  Болезнь:       ") << stat.hoursIllness << QString::fromUtf8(" ч.") << endl;

        stream << endl;
    }
}
