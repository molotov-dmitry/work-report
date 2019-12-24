#ifndef REPORTBUILDERTOTALREDUCED_H
#define REPORTBUILDERTOTALREDUCED_H

#include <QList>
#include <QTextStream>

#include "reportentry.h"

void BuildReportReduced(const QList<ReportEntry>& list, QTextStream &stream);

#endif // REPORTBUILDERTOTALREDUCED_H
