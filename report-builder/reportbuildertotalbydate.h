#ifndef REPORTBUILDERTOTALBYDATE_H
#define REPORTBUILDERTOTALBYDATE_H

#include <QList>
#include <QTextStream>

#include "reportentry.h"

void BuildReportTotalBydate(const QList<ReportEntry>& list, QTextStream &stream);

#endif // REPORTBUILDERTOTALBYDATE_H
