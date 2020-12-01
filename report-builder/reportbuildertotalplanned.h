#ifndef REPORTBUILDERTOTALPLANNED_H
#define REPORTBUILDERTOTALPLANNED_H

#include <QList>
#include <QTextStream>

#include "reportentry.h"

void BuildReportPlanned(const QList<ReportEntry>& list, QTextStream &stream);

#endif // REPORTBUILDERTOTALPLANNED_H
