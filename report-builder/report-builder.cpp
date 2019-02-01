#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>

#include "values.h"
#include "reportentry.h"

#include "reportbuilderstatistics.h"
#include "reportbuildertotalbydate.h"
#include "reportbuildertotalreduced.h"

uint findId(const QString& value, const Values* values, uint count)
{
    for (uint i = 0; i < count; ++i)
    {
        QString displayValue = QString::fromUtf8(values[i].displayValue);

        if (value == displayValue)
        {
            return i;
        }
    }

    return count;
}

bool toReportEntry(const QStringList& line, ReportEntry& entry)
{
    enum Columns
    {
        COL_NAME,
        COL_DATES,
        COL_TYPE,
        COL_HOURS,
        COL_PROJECT,
        COL_PRODUCT,
        COL_ACTION,
        COL_DESCRIPTION,
        COL_RESULT,

        COL_COUNT
    };

    if (line.size() != COL_COUNT)
    {
        qCritical() << "Wrong column count: " << line.size();
        return false;
    }

    //// Get common fields =====================================================

    bool ok;

    entry.name = line.at(COL_NAME);

    entry.hours = line.at(COL_HOURS).toUInt(&ok);
    if (not ok)
    {
        qCritical() << "Wrong hours value: " << line.at(COL_HOURS);
        return false;
    }

    entry.type = (TaskType)findId(line.at(COL_TYPE), gValuesTaskTypes, TASK_COUNT);

    if (entry.type == TASK_COUNT)
    {
        qCritical() << "Wrong type value: " << line.at(COL_TYPE);
        return false;
    }

    //// Get action task fields ================================================

    if (entry.type == TASK_ACTION)
    {
        entry.project       = line.at(COL_PROJECT);
        entry.product       = line.at(COL_PRODUCT);
        entry.description   = line.at(COL_DESCRIPTION);

        //// Action ----------------------------------------------------------------

        entry.action = (TaskActionType)findId(line.at(COL_ACTION), gValuesActionTypes, ACTION_COUNT);

        if (entry.action == ACTION_COUNT)
        {
            qCritical() << "Wrong action value: " << line.at(COL_ACTION);
            return false;
        }

        //// Result ----------------------------------------------------------------

        entry.result = (TaskResult)findId(line.at(COL_RESULT), gValuesResults, RESULT_COUNT);;

        if (entry.result == RESULT_COUNT)
        {
            qCritical() << "Wrong result value: " << line.at(COL_RESULT);
            return false;
        }
    }

    //// =======================================================================

    return true;
}

bool splitReportLine(const QString& line, QStringList &lines)
{
    lines.clear();

    bool quotePrev = false;
    bool quoted    = false;

    QString currentString;

    foreach (const QChar& c, line)
    {
        if (currentString.isEmpty() && not quoted && c == '"')
        {
            quoted = true;
            continue;
        }

        if (quoted)
        {
            if (quotePrev)
            {
                if (c == '"')
                {
                    quotePrev = false;

                    currentString.append(c);
                }
                else if (c == ';')
                {
                    lines.append(currentString);
                    currentString.clear();

                    quotePrev = false;
                    quoted    = false;

                    continue;
                }
                else
                {
                    qCritical() << "Wrong symbol after quote: " << c;
                    return false;
                }
            }
            else
            {
                if (c == '"')
                {
                    quotePrev = true;
                }
                else
                {
                    currentString.append(c);
                }
            }
        }
        else
        {
            if (c == ';')
            {
                lines.append(currentString);
                currentString.clear();

                quotePrev = false;
                quoted    = false;

                continue;
            }
            else
            {
                currentString.append(c);
            }
        }

    }

    lines.append(currentString);

    return true;
}

bool readReport(const QString& reportPath, QList<ReportEntry>& entries)
{
    QFile reportFile(reportPath);

    if (not reportFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open report file";
        return false;
    }

    bool readTitle = false;

    int lineNumber  = 1;

    QTextStream stream(&reportFile);
    while (not stream.atEnd())
    {
        QString line = stream.readLine();

        if (not readTitle)
        {
            ++lineNumber;

            readTitle = true;
            continue;
        }

        QStringList lines;

        if (not splitReportLine(line, lines))
        {
            qCritical() << "In line " << lineNumber;

            return false;
        }

        ReportEntry entry;

        if (not toReportEntry(lines, entry))
        {
            qCritical() << "In line " << lineNumber;

            return false;
        }

        entries.append(entry);

        ++lineNumber;

    }

    return true;

}

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
        if (not readReport(report, entries))
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
        qCritical() << "Failed to open total.csv fro write";
        return 1;
    }

    QTextStream streamTotalByDate(&fileReportTotal);
    streamTotalByDate.setGenerateByteOrderMark(true);

    BuildReportTotalBydate(entries, streamTotalByDate);

    //// Build reduced =========================================================

    QFile fileReportReduced(reportDirectory.absoluteFilePath("reduced.csv"));
    if (not fileReportReduced.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open total.csv fro write";
        return 1;
    }

    QTextStream streamReduced(&fileReportReduced);
    streamReduced.setGenerateByteOrderMark(true);

    BuildReportReduced(entries, streamReduced);

    //// =======================================================================

    return 0;
}
