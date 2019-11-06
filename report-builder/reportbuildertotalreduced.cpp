#include "reportbuildertotalreduced.h"

static QString toCsvValue(QString text)
{
    bool quoteString = text.contains(';') || text.contains('"') || text.contains(",");

    text.replace('"', "\"\"");

    if (quoteString)
    {
        text.append('"');
        text.prepend('"');
    }

    return text;

}

void BuildReportReduced(const QList<ReportEntry> &list, QTextStream &stream)
{
    QMap<QString, QMap<QString, QMap<QString, QMap<QString, int> > > > map;

    foreach (const ReportEntry& e, list)
    {
        QString worker;
        QString project;
        QString product;
        QString action;

        worker = e.name;

        if (e.type == TASK_ACTION)
        {
            project = e.project;
            product = e.product;

            if (e.action == ACTION_OTHER)
            {
                action = e.description;
            }
            else
            {
                action = QString::fromUtf8(gValuesActionTypes[e.action].displayValue);
            }
        }
        else
        {
            action = QString::fromUtf8(gValuesTaskTypes[e.type].displayValue);
        }

        map[worker][project][product][action] += e.hours;
    }

    foreach (const QString& worker, map.keys())
    {
        foreach (const QString& project, map.value(worker).keys())
        {
            foreach (const QString& product, map.value(worker).value(project).keys())
            {
                foreach (const QString& action, map.value(worker).value(project).value(product).keys())
                {
                    int hours = map.value(worker).value(project).value(product).value(action);

                    stream << toCsvValue(worker) << ";";
                    stream << hours << ";";
                    stream << toCsvValue(project) << ";";
                    stream << toCsvValue(product) << ";";
                    stream << toCsvValue(action);

                    stream << endl;
                }
            }
        }
    }

    stream << endl;
}
