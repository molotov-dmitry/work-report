#include "reportbuildertotalplanned.h"
#include "common/reportexportcsv.h"

void BuildReportPlanned(const QList<ReportEntry> &list, QTextStream &stream)
{
    QMap<QString, QMap<QString, QMap<QString, int> > > map;

    foreach (const ReportEntry& e, list)
    {
        QString worker;
        QString project;
        QString product;
        QString plan;

        worker = e.name;

        if (e.type == TASK_ACTION)
        {
            project = e.project;
            product = e.product;

            plan = e.plan;
        }
        else
        {
            plan = QString::fromUtf8(gValuesTaskTypes[e.type].displayValue);
        }

        map[worker][project][plan] += e.hours;
    }

    foreach (const QString& worker, map.keys())
    {
        foreach (const QString& project, map.value(worker).keys())
        {
            foreach (const QString& plan, map.value(worker).value(project).keys())
            {
                int hours = map.value(worker).value(project).value(plan);

                stream << ReportExportCsv::toCsvValue(worker) << ";";
                stream << hours << ";";
                stream << ReportExportCsv::toCsvValue(project) << ";";
                stream << ReportExportCsv::toCsvValue(plan);

                stream << endl;
            }
        }
    }

    stream << endl;
}
