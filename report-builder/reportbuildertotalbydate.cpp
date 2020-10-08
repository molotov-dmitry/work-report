#include "reportbuildertotalbydate.h"
#include "common/reportexportcsv.h"

#include <QSet>

void BuildReportTotalBydate(const QList<ReportEntry> &list, QTextStream& stream)
{
    foreach (const ReportEntry& e, list)
    {
        stream << ReportExportCsv::toCsvValue(e.name) << ";";
        stream << ReportExportCsv::toCsvValue(e.from.toString("dd.MM.yyyy") + " - " + e.to.toString("dd.MM.yyyy")) << ";";
        stream << ReportExportCsv::toCsvValue(QString::fromUtf8(gValuesTaskTypes[e.type].displayValue)) << ";";
        stream << e.hours << ";";

        if (e.type == TASK_ACTION)
        {
            stream << ReportExportCsv::toCsvValue(e.project) << ";";
            stream << ReportExportCsv::toCsvValue(e.product) << ";";
            stream << ReportExportCsv::toCsvValue(QString::fromUtf8(gValuesActionTypes[e.action].displayValue)) << ";";
            stream << ReportExportCsv::toCsvValue(e.description) << ";";
            stream << ReportExportCsv::toCsvValue(QString::fromUtf8(gValuesResults[e.result].displayValue));
        }
        else
        {
            stream << ";";
            stream << ";";
            stream << ";";
            stream << ";";
        }

        stream << endl;
    }
}

QList<ReportEntry> squashReportList(const QList<ReportEntry>& list)
{
    QList<ReportEntry> result;
    QSet<int> skippedIndexes;

    for (int i = 0; i < list.size(); ++i)
    {
        if (skippedIndexes.contains(i))
        {
            continue;
        }

        ReportEntry re1 = list.at(i);

        for (int j = i + 1; j < list.size(); ++j)
        {
            if (skippedIndexes.contains(j))
            {
                continue;
            }

            ReportEntry re2 = list.at(j);

            if (re1.name != re2.name)
            {

            }
            else if (re1.type != re2.type)
            {

            }
            else if (re1.project != re2.project)
            {

            }
            else if (re1.product != re2.product)
            {

            }
            else if (re1.action != re2.action)
            {

            }
            else if (re1.description != re2.description)
            {

            }
            else if (re1.plan != re2.plan)
            {

            }
            else
            {
                re1.hours += re2.hours;

                if (re2.from < re1.from)
                {
                    re1.from = re2.from;
                }
                if (re2.to > re1.to)
                {
                    re1.to = re2.to;
                }
                if (re2.result < re1.result)
                {
                    re1.result = re2.result;
                }

                skippedIndexes.insert(j);
            }
        }

        result.append(re1);
    }

    return result;
}
