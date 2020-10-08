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

    QStringList reports;

    QDirIterator it(reportDirectory.absolutePath(), QStringList() << QString::fromUtf8("Отчет ??.??.???? - ??.??.????.csv"), QDir::Files, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
    while (it.hasNext())
    {
        reports << it.next();
    }

    //// =======================================================================

    QList<ReportEntry> entries;

    //// =======================================================================

    foreach (const QString& report, reports)
    {
        ReportImport import;

        if (not import.readReport(report, entries))
        {
            qCritical() << "In file " << report;
            return 1;
        }
    }

    //// Sort reports ==========================================================

    std::sort(entries.begin(), entries.end(), compareReportEntries);

    //// Print statistics ======================================================

    printStatistics(entries);

    //// Build total by date ===================================================

    QFile fileReportTotal(reportDirectory.absoluteFilePath("total.csv"));
    if (not fileReportTotal.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open total.csv for write";
        return 1;
    }

    QTextStream streamTotalByDate(&fileReportTotal);
    streamTotalByDate.setGenerateByteOrderMark(true);

    BuildReportTotalBydate(entries, streamTotalByDate);

    //// Build reduced =========================================================

    QFile fileReportReduced(reportDirectory.absoluteFilePath("reduced.csv"));
    if (not fileReportReduced.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open total.csv for write";
        return 1;
    }

    QTextStream streamReduced(&fileReportReduced);
    streamReduced.setGenerateByteOrderMark(true);

    BuildReportReduced(entries, streamReduced);

    //// =======================================================================

    return 0;
}
