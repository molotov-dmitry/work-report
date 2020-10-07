#include "reportbuildertotalbydate.h"
#include "common/reportexportcsv.h"

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
