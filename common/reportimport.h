#ifndef REPORTIMPORT_H
#define REPORTIMPORT_H

#include <QString>

#include "reportentry.h"

class ReportImport
{
public:
    ReportImport();

    bool readReport(const QString& reportPath, QList<ReportEntry>& entries);

    QString lastError() const;
    int     lastErrorLine() const;

private:

    QString mLastError;
    int     mLastErrorLine;

    uint findId(const QString& value, const Values* values, uint count);
    bool toReportEntry(const QStringList& line, ReportEntry& entry);
    bool splitReportLine(const QString& line, QStringList &lines);

};

#endif // REPORTIMPORT_H
