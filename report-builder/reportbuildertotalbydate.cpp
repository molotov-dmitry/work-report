#include "reportbuildertotalbydate.h"

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

void BuildReportTotalBydate(const QList<ReportEntry> &list, QTextStream& stream)
{
    foreach (const ReportEntry& e, list)
    {
        stream << toCsvValue(e.name) << ";";
        stream << toCsvValue(e.from.toString("dd.MM.yyyy") + " - " + e.to.toString("dd.MM.yyyy")) << ";";
        stream << toCsvValue(QString::fromUtf8(gValuesTaskTypes[e.type].displayValue)) << ";";
        stream << e.hours << ";";

        if (e.type == TASK_ACTION)
        {
            stream << toCsvValue(e.project) << ";";
            stream << toCsvValue(e.product) << ";";
            stream << toCsvValue(QString::fromUtf8(gValuesActionTypes[e.action].displayValue)) << ";";
            stream << toCsvValue(e.description) << ";";
            stream << toCsvValue(QString::fromUtf8(gValuesResults[e.result].displayValue));
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
