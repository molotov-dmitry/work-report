#ifndef REPORTBUILDERSTATISTICS_H
#define REPORTBUILDERSTATISTICS_H

#include <QList>

#include "reportentry.h"

void printStatistics(const QList<ReportEntry>& plan,
                     const QList<ReportEntry>& actual);

#endif // REPORTBUILDERSTATISTICS_H
