#include "reportexportcsv.h"

QString ReportExportCsv::toCsvValue(QString text)
{
    bool quoteString = text.contains(';') || text.contains('"') || text.contains(",");

    text.replace('"', "\"\"");
    text.replace("\n", "\r");

    if (quoteString)
    {
        text.append('"');
        text.prepend('"');
    }

    return text;
}

QString ReportExportCsv::toCsvValue(const char* utf8ConstString)
{
    return toCsvValue(QString::fromUtf8(utf8ConstString));
}
