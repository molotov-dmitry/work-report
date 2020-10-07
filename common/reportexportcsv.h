#ifndef REPORTEXPORTCSV_H
#define REPORTEXPORTCSV_H

#include <QString>

class ReportExportCsv
{
private:
    ReportExportCsv() = delete;
    ReportExportCsv(const ReportExportCsv& other) = delete;
    ReportExportCsv operator=(const ReportExportCsv& other) = delete;

public:

    static QString toCsvValue(QString text);
    static QString toCsvValue(const char* utf8ConstString);


};

#endif // REPORTEXPORTCSV_H
