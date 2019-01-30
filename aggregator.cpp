#include <QApplication>

#include <QDir>
#include <QDirIterator>
#include <QTextStream>

#include "values.h"

struct ReportEntry
{
    QString name;

//    QDate   from;
//    QDate   to;

    uint    type;
    uint    hours;

    QString project;
    QString product;
    uint    action;
    QString description;
    uint    result;
};

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
        return false;
    }

    //// Get common fields =====================================================

    bool ok;

    entry.name = line.at(COL_NAME);

    entry.hours = line.at(COL_HOURS).toUInt(&ok);
    if (not ok)
    {
        return false;
    }

    entry.type = findId(line.at(COL_TYPE), gValuesTaskTypes, TASK_COUNT);

    if (entry.type == TASK_COUNT)
    {
        return false;
    }

    //// Get action task fields ================================================

    if (entry.type == TASK_ACTION)
    {
        entry.project       = line.at(COL_PROJECT);
        entry.product       = line.at(COL_PRODUCT);
        entry.description   = line.at(COL_DESCRIPTION);

        //// Action ----------------------------------------------------------------

        entry.action = findId(line.at(COL_ACTION), gValuesActionTypes, ACTION_COUNT);

        if (entry.action == ACTION_COUNT)
        {
            return false;
        }

        //// Result ----------------------------------------------------------------

        entry.result = findId(line.at(COL_RESULT), gValuesResults, RESULT_COUNT);;

        if (entry.result == RESULT_COUNT)
        {
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
        //TODO: error
        return false;
    }

    bool readTitle = false;

    QTextStream stream(&reportFile);
    while (not stream.atEnd())
    {
        QString line = stream.readLine();

        if (not readTitle)
        {
            readTitle = true;
            continue;
        }

        QStringList lines;

        if (not splitReportLine(line, lines))
        {
            return false;
        }

        ReportEntry entry;

        if (not toReportEntry(lines, entry))
        {
            return false;
        }

        entries.append(entry);

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
                //TODO: error
            }
            if (not reportDirectory.cd(QString::fromUtf8(argv[i + 1])))
            {
                //TODO: error
                return 1;
            }
        }
    }

    //// Get report paths list =================================================

    QStringList reports;

    QDirIterator it(reportDirectory.absolutePath(), QStringList() << QString::fromUtf8("Отчет ??.??.???? - ??.??.????.csv"), QDir::Files, QDirIterator::Subdirectories);
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
            return 1;
        }
    }

    //// =======================================================================

    return 0;
}
