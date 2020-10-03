#ifndef REPORTENTRY_H
#define REPORTENTRY_H

#include <QString>
#include <QDate>

#include "values.h"

struct ReportEntry
{
    QString         name;

    QDate           from;
    QDate           to;

    TaskType        type;
    uint            hours;

    QString         project;
    QString         product;
    TaskActionType  action;
    QString         description;
    TaskResult      result;

    QString         plan;
};

bool compareReportEntries(const ReportEntry& re1, const ReportEntry& re2);

#endif // REPORTENTRY_H
