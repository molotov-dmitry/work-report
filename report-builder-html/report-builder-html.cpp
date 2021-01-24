#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>

#include <QMap>

#include "values.h"
#include "common/reportentry.h"
#include "common/reportimport.h"

struct Hours
{
    quint64 planned;
    quint64 actual;
};

static Hours mHoursTotalOverall;
static QMap<QString, Hours> mHoursTotalWorker;
static QMap<QString, Hours> mHourstTotalProject;
static QMap<QString, QMap<QString, Hours> > mHourstTotalPlan;
static QMap<QString, QMap<QString, QMap<QString, Hours> > > mHoursStat;

int main(int argc, char *argv[])
{
    //// Get parameters ========================================================

    QDir    reportDirectory;
    QString commonReportPath;
    QString reducedReportPath;

    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            if (i == argc - 1)
            {
                qCritical() << "Missing value for '-d' argument";
            }
            if (not reportDirectory.cd(QString::fromUtf8(argv[i + 1])))
            {
                qCritical() << "Failed to change directory";
                return 1;
            }
        }
    }

    //// Get entries paths list ================================================

    QStringList reportsPlanned;

    QDirIterator itp(reportDirectory.absolutePath(), QStringList() << QString::fromUtf8("План \?\?\?\?-\?\?.csv"), QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    while (itp.hasNext())
    {
        reportsPlanned << itp.next();
    }

    QStringList reportsActual;

    QDirIterator ita(reportDirectory.absolutePath(), QStringList() << QString::fromUtf8("Отчет \?\?.\?\?.\?\?\?\? - \?\?.\?\?.\?\?\?\?.csv"), QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    while (ita.hasNext())
    {
        reportsActual << ita.next();
    }

    //// Load entries ==========================================================

    //// Planned ---------------------------------------------------------------

    foreach (const QString& report, reportsPlanned)
    {
        QList<ReportEntry> entries;
        ReportImport import;

        if (not import.readReport(report, entries, true))
        {
            qCritical() << import.lastError();
            qCritical() << "In file " << report;
            return 1;
        }

        foreach (const ReportEntry& entry, entries)
        {
            if (entry.type != TASK_ACTION)
            {
                continue;
            }

            mHoursStat[entry.project][entry.plan][entry.name].planned += entry.hours;
            mHourstTotalProject[entry.project].planned                += entry.hours;
            mHourstTotalPlan[entry.project][entry.plan].planned       += entry.hours;
            mHoursTotalWorker[entry.name].planned                     += entry.hours;
            mHoursTotalOverall.planned                                += entry.hours;
        }
    }

    //// Actual ----------------------------------------------------------------

    foreach (const QString& report, reportsActual)
    {
        QList<ReportEntry> entries;
        ReportImport import;

        if (not import.readReport(report, entries, false))
        {
            qCritical() << import.lastError();
            qCritical() << "In file " << report;
            return 1;
        }

        foreach (const ReportEntry& entry, entries)
        {
            if (entry.type != TASK_ACTION)
            {
                continue;
            }

            mHoursStat[entry.project][entry.plan][entry.name].actual += entry.hours;
            mHourstTotalProject[entry.project].actual                += entry.hours;
            mHourstTotalPlan[entry.project][entry.plan].actual       += entry.hours;
            mHoursTotalWorker[entry.name].actual                     += entry.hours;
            mHoursTotalOverall.actual                                += entry.hours;
        }
    }

    //// =======================================================================

    return 0;
}
