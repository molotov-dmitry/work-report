#include "reportimport.h"

#include <QString>
#include <QTextStream>
#include <QFile>

#include "values.h"
#include "reportentry.h"

ReportImport::ReportImport()
{

}

bool ReportImport::readReport(const QString &reportPath, QList<ReportEntry> &entries)
{
    mLastError.clear();
    mLastErrorLine = -1;

    QFile reportFile(reportPath);

    if (not reportFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mLastError = "Failed to open report file: " + reportFile.errorString();

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
            mLastErrorLine = lineNumber;

            return false;
        }

        ReportEntry entry;

        if (not toReportEntry(lines, entry))
        {
            mLastErrorLine = lineNumber;

            return false;
        }

        entries.append(entry);

        ++lineNumber;

    }

    return true;
}

QString ReportImport::lastError() const
{
    return mLastError;
}

int ReportImport::lastErrorLine() const
{
    return mLastErrorLine;
}

uint ReportImport::findId(const QString &value, const Values *values, uint count)
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

bool ReportImport::toReportEntry(const QStringList &line, ReportEntry &entry)
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
        mLastError = QString::asprintf("Wrong column count: %d", line.size());
        return false;
    }

    //// Get common fields =====================================================

    bool ok;

    entry.name = line.at(COL_NAME);

    entry.hours = line.at(COL_HOURS).toUInt(&ok);
    if (not ok)
    {
        mLastError = "Wrong hours value: " + line.at(COL_HOURS);
        return false;
    }

    entry.type = (TaskType)findId(line.at(COL_TYPE), gValuesTaskTypes, TASK_COUNT);

    if (entry.type == TASK_COUNT)
    {
        mLastError = "Wrong type value: " + line.at(COL_TYPE);
        return false;
    }

    //// Get dates =============================================================

    QStringList dateRange = line.at(COL_DATES).split('-');

    if (dateRange.count() != 2)
    {
        mLastError = "Wrong dates range: " + line.at(COL_DATES);
        return false;
    }

    entry.from = QDate::fromString(dateRange.at(0).trimmed(), "dd.MM.yyyy");
    entry.to   = QDate::fromString(dateRange.at(1).trimmed(), "dd.MM.yyyy");

    if (not entry.from.isValid())
    {
        mLastError = "Wrong start date: " + line.at(COL_DATES);
        return false;
    }

    if (not entry.to.isValid())
    {
        mLastError = "Wrong end date: " + line.at(COL_DATES);
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
            mLastError = "Wrong action value: " + line.at(COL_ACTION);
            return false;
        }

        //// Result ----------------------------------------------------------------

        entry.result = (TaskResult)findId(line.at(COL_RESULT), gValuesResults, RESULT_COUNT);;

        if (entry.result == RESULT_COUNT)
        {
            mLastError = "Wrong result value: " + line.at(COL_RESULT);
            return false;
        }
    }

    //// =======================================================================

    return true;
}

bool ReportImport::splitReportLine(const QString &line, QStringList &lines)
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
                    mLastError = QString::fromUtf8("Wrong symbol after quote: ") + c;
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
