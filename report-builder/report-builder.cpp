#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>

#include "values.h"
#include "common/reportentry.h"
#include "common/reportimport.h"

#include "reportbuilderstatistics.h"
#include "reportbuildertotalbydate.h"
#include "reportbuildertotalreduced.h"
#include "reportbuildertotalplanned.h"

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

    //// Get report paths list =================================================

    QStringList reportsPlanned;

    QDirIterator itp(reportDirectory.absolutePath(), QStringList() << QString::fromUtf8("План \?\?\?\?-\?\?.csv"), QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    while (itp.hasNext())
    {
        reportsPlanned << itp.next();
    }

    QStringList reportsActual;

    QDirIterator ita(reportDirectory.absolutePath(), QStringList() << QString::fromUtf8("Отчет ??.??.???? - ??.??.????.csv"), QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    while (ita.hasNext())
    {
        reportsActual << ita.next();
    }

    //// Read plans/reports ====================================================

    QList<ReportEntry> entriesPlanned;
    QList<ReportEntry> entriesActual;

    foreach (const QString& report, reportsPlanned)
    {
        ReportImport import;

        if (not import.readReport(report, entriesPlanned, true))
        {
            qCritical() << "In file " << report;
            return 1;
        }
    }

    foreach (const QString& report, reportsActual)
    {
        ReportImport import;

        if (not import.readReport(report, entriesActual))
        {
            qCritical() << "In file " << report;
            return 1;
        }
    }

    //// Sort reports ==========================================================

    std::sort(entriesActual.begin(), entriesActual.end(), compareReportEntries);

    //// Print statistics ======================================================

    printStatistics(entriesPlanned, entriesActual);

    //// Build total by date ===================================================

    QFile fileReportTotal(reportDirectory.absoluteFilePath("total.csv"));
    if (not fileReportTotal.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open total.csv for write";
        return 1;
    }

    QTextStream streamTotalByDate(&fileReportTotal);
    streamTotalByDate.setGenerateByteOrderMark(true);

    BuildReportTotalBydate(entriesActual, streamTotalByDate);

    //// Build total by date ===================================================

    QFile fileReportCompact(reportDirectory.absoluteFilePath("compact.csv"));
    if (not fileReportCompact.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open compact.csv for write";
        return 1;
    }

    QTextStream streamCompactByDate(&fileReportCompact);
    streamTotalByDate.setGenerateByteOrderMark(true);

    BuildReportTotalBydate(squashReportList(entriesActual), streamCompactByDate);

    //// Build reduced =========================================================

    QFile fileReportReduced(reportDirectory.absoluteFilePath("reduced.csv"));
    if (not fileReportReduced.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open total.csv for write";
        return 1;
    }

    QTextStream streamReduced(&fileReportReduced);
    streamReduced.setGenerateByteOrderMark(true);

    BuildReportReduced(entriesActual, streamReduced);

    //// Build planned =========================================================

    QFile fileReportPlanned(reportDirectory.absoluteFilePath("planned.csv"));
    if (not fileReportPlanned.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open total.csv for write";
        return 1;
    }

    QTextStream streamPlanned(&fileReportPlanned);
    streamPlanned.setGenerateByteOrderMark(true);

    BuildReportPlanned(entriesActual, streamPlanned);

    //// =======================================================================

    return 0;
}
