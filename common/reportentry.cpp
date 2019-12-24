#include "reportentry.h"

bool compareReportEntries(const ReportEntry &re1, const ReportEntry &re2)
{
    if (re1.name != re2.name)
    {
        return (re1.name < re2.name);
    }
    else if (re1.type != re2.type)
    {
        return (re1.type < re2.type);
    }
    else if (re1.project != re2.project)
    {
        return (re1.project < re2.project);
    }
    else if (re1.product != re2.product)
    {
        return (re1.product < re2.product);
    }
    else if (re1.action != re2.action)
    {
        return (re1.action < re2.action);
    }
    else if (re1.description != re2.description)
    {
        return (re1.description < re2.description);
    }
    else if (re1.hours != re2.hours)
    {
        return (re1.hours < re2.hours);
    }
    else
    {
        return (re1.result < re2.result);
    }
}
