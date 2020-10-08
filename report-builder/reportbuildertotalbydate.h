#ifndef REPORTBUILDERTOTALBYDATE_H
#define REPORTBUILDERTOTALBYDATE_H

#include <QList>
#include <QTextStream>

#include "reportentry.h"

QList<ReportEntry> squashReportList(const QList<ReportEntry>& list);

void BuildReportTotalBydate(const QList<ReportEntry>& list, QTextStream &stream);

#endif // REPORTBUILDERTOTALBYDATE_H
