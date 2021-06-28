#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QXmlStreamWriter>

#include <QMap>
#include <QSet>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "values.h"
#include "common/reportentry.h"
#include "common/reportimport.h"
#include "settings.h"

struct Hours
{
    quint64 planned;
    quint64 actual;
};

static Hours mHoursTotalOverall;
static QMap<QString, Hours> mHoursTotalWorker;
static QMap<QString, Hours> mHourstTotalProject;
static QMap<QString, QMap<QString, Hours> > mHourstTotalTask;
static QMap<QString, QMap<QString, QMap<QString, Hours> > > mHoursStat;

static QString sNameDepartment  = QString::fromUtf8("отдела разработки программного обеспечения");
static QString sNameDivision    = QString::fromUtf8("по разработкам");

static QString sLeadDefault     = QString("Фамилия И. О.");

static QMap<QString, QString> sProjectLeads;

bool compareStrings(const QString& s1, const QString& s2)
{
    return (s1 < s2);
}

void buildReportHtml(const QDate& date, const QDir &dir, bool onlyPlan)
{
    //// Extract style =========================================================

    QString style;
    {
        QFile styleFile(":/css/report.css");
        styleFile.open(QIODevice::ReadOnly);
        style = QString::fromUtf8(styleFile.readAll());
    }

    //// Open file =============================================================

    QFile file;
    {
        QString fileName;

        if (onlyPlan)
        {
            fileName = dir.absoluteFilePath("plan.html");
        }
        else
        {
            fileName = dir.absoluteFilePath("report.html");
        }

        file.setFileName(fileName);
        file.open(QIODevice::WriteOnly);
    }

    //// Write HTML ============================================================

    QLocale locale = QLocale::system();
    QString title = "Данные о занятости сотрудников " +
                    sNameDepartment +
                    " за " + locale.standaloneMonthName(date.month()) +
                    " " + QString::number(date.year()) + " г.";

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);

    stream.writeStartElement("html");

    //// Head ==================================================================

    stream.writeStartElement("head");
    stream.writeTextElement("title", title);
    stream.writeStartElement("meta");
    stream.writeAttribute("charset", "UTF-8");
    stream.writeEndElement();
    stream.writeTextElement("style", style);
    stream.writeEndElement();

    //// Body ==================================================================

    stream.writeStartElement("body");

    stream.writeStartElement("p");
    stream.writeAttribute("style", "position:relative; left: 50%");
    stream.writeCharacters("Приложение 6");
    stream.writeStartElement("br");
    stream.writeEndElement();
    stream.writeCharacters("к приказу об отчетности");
    stream.writeStartElement("br");
    stream.writeEndElement();
    stream.writeCharacters("о выполнении работ по заказам в АО «РЦЗИ «ФОРТ»");
    stream.writeEndElement();


    stream.writeTextElement("p", "№ ________ от __.__.20__ г.");

    stream.writeStartElement("h4");
    stream.writeAttribute("style", "text-align:center;");
    stream.writeCharacters(title);
    stream.writeEndElement();

    stream.writeStartElement("table");
    stream.writeAttribute("style", "vertical-align: middle;text-align: center");

    //// Row 1: Workers --------------------------------------------------------

    stream.writeStartElement("tr");

    stream.writeStartElement("td");
    stream.writeAttribute("rowspan", "3");
    stream.writeAttribute("class", "rotate");
    stream.writeTextElement("div", "№ п/п");
    stream.writeEndElement();

    stream.writeStartElement("td");
    stream.writeAttribute("rowspan", "3");
    stream.writeCharacters("Заказ");
    stream.writeEndElement();

    stream.writeStartElement("td");
    stream.writeAttribute("rowspan", "3");
    stream.writeCharacters("Работа");
    stream.writeEndElement();

    QStringList workers = mHoursTotalWorker.keys();
    std::sort(workers.begin(), workers.end(), compareStrings);

    foreach (const QString& worker, workers)
    {
        stream.writeStartElement("td");
        stream.writeAttribute("rowspan", "2");
        stream.writeAttribute("colspan", "2");
        stream.writeAttribute("class", "rotate");
        stream.writeAttribute("height", "150mm"); //TODO: calculate
        stream.writeTextElement("div", worker);
        stream.writeEndElement();
    }

    stream.writeStartElement("td");
    stream.writeAttribute("colspan", "6");
    stream.writeCharacters("Трудоёмкость, чел./дн.");
    stream.writeEndElement();

    stream.writeEndElement();

    //// Row 2: project summary ------------------------------------------------

    stream.writeStartElement("tr");

    stream.writeStartElement("td");
    stream.writeAttribute("colspan", "2");
    stream.writeAttribute("class", "rotate");
    stream.writeStartElement("div");
    stream.writeCharacters("Итого по");
    stream.writeStartElement("br");
    stream.writeEndElement();
    stream.writeCharacters("работе");
    stream.writeEndElement();
    stream.writeEndElement();

    stream.writeStartElement("td");
    stream.writeAttribute("colspan", "2");
    stream.writeAttribute("class", "rotate");
    stream.writeStartElement("div");
    stream.writeCharacters("Итого по");
    stream.writeStartElement("br");
    stream.writeEndElement();
    stream.writeCharacters("проекту");
    stream.writeEndElement();
    stream.writeEndElement();

    stream.writeStartElement("td");
    stream.writeAttribute("colspan", "2");
    stream.writeTextElement("div", "Руководитель заказа (по приказу)");
    stream.writeEndElement();

    stream.writeEndElement();

    //// Row 3: planned and actual header --------------------------------------

    stream.writeStartElement("tr");
    stream.writeAttribute("height", "70 mm");

    for (int i = 0; i < (workers.count() + 2) * 2; ++i)
    {
        QString value;

        if (i % 2 == 0)
        {
            value = "план";
        }
        else
        {
            value = "факт";
        }

        stream.writeStartElement("td");
        stream.writeAttribute("class", "rotate");
        stream.writeTextElement("div", value);
        stream.writeEndElement();
    }

    stream.writeStartElement("td");
    stream.writeAttribute("class", "rotate");
    stream.writeTextElement("div", "ФИО");
    stream.writeEndElement();

    stream.writeStartElement("td");
    stream.writeAttribute("class", "rotate");
    stream.writeTextElement("div", "Подпись");
    stream.writeEndElement();

    stream.writeEndElement();

    //// Data rows: hours ------------------------------------------------------

    int orderNum = 1;

    //// Generate project leads map --------------------------------------------

    QMap<QString, QStringList> projectLeads;
    projectLeads.insert(sLeadDefault, mHourstTotalProject.keys());

    foreach (const QString& project, sProjectLeads.keys())
    {
        projectLeads[sLeadDefault].removeAll(project);
        projectLeads[sProjectLeads.value(project)].append(project);
    }

    QStringList leadsList = projectLeads.keys();
    std::sort(leadsList.begin(), leadsList.end(), compareStrings);

    //// -----------------------------------------------------------------------

    foreach (const QString& lead, leadsList)
    {
        bool firstProject = true;
        QStringList projects = projectLeads.value(lead);
        std::sort(projects.begin(), projects.end(), compareStrings);

        int totalTasks = 0;

        foreach (const QString &project, projects)
        {
            totalTasks += mHourstTotalTask.value(project).count();
        }

        foreach (const QString &project, projects)
        {
            bool firstTask = true;
            QStringList tasks = mHourstTotalTask.value(project).keys();

            stream.writeStartElement("tr");

            stream.writeTextElement("td", QString::number(orderNum++));

            stream.writeStartElement("td");
            stream.writeAttribute("rowspan", QString::number(tasks.count()));
            stream.writeCharacters(project);
            stream.writeEndElement();

            foreach (const QString &task, tasks)
            {
                if (not firstTask)
                {
                    stream.writeStartElement("tr");
                    stream.writeTextElement("td", QString::number(orderNum++));
                }

                stream.writeStartElement("td");
                stream.writeAttribute("align", "left");
                stream.writeCharacters(task);
                stream.writeEndElement();

                foreach (const QString& worker, workers)
                {
                    Hours hours = mHoursStat.value(project).value(task).value(worker);

                    stream.writeStartElement("td");
                    if (hours.planned != 0)
                    {
                        stream.writeCharacters(QString::number(hours.planned));
                    }
                    stream.writeEndElement();

                    stream.writeStartElement("td");
                    if ((hours.planned != 0 || hours.actual != 0) && not onlyPlan)
                    {
                        stream.writeCharacters(QString::number(hours.actual));
                    }
                    stream.writeEndElement();
                }

                stream.writeTextElement("td", QString::number(mHourstTotalTask.value(project).value(task).planned));

                stream.writeStartElement("td");
                if (not onlyPlan)
                {
                    stream.writeCharacters(QString::number(mHourstTotalTask.value(project).value(task).actual));
                }
                stream.writeEndElement();

                if (firstTask)
                {
                    stream.writeStartElement("td");
                    stream.writeAttribute("rowspan", QString::number(tasks.count()));
                    stream.writeCharacters(QString::number(mHourstTotalProject.value(project).planned));
                    stream.writeEndElement();

                    stream.writeStartElement("td");
                    stream.writeAttribute("rowspan", QString::number(tasks.count()));
                    if (not onlyPlan)
                    {
                        stream.writeCharacters(QString::number(mHourstTotalProject.value(project).actual));
                    }
                    stream.writeEndElement();
                }

                if (firstProject)
                {
                    stream.writeStartElement("td");
                    stream.writeAttribute("rowspan", QString::number(totalTasks));
                    stream.writeAttribute("class", "rotate");
                    stream.writeAttribute("height", "100 mm");
                    stream.writeTextElement("div", lead);
                    stream.writeEndElement();

                    stream.writeStartElement("td");
                    stream.writeAttribute("rowspan", QString::number(totalTasks));
                    stream.writeEndElement();
                }

                stream.writeEndElement();

                firstTask    = false;
                firstProject = false;
            }
        }
    }

    //// Total workers hours ---------------------------------------------------

    stream.writeStartElement("tr");

    stream.writeStartElement("td");
    stream.writeAttribute("align", "left");
    stream.writeAttribute("colspan", "3");
    stream.writeCharacters("Итого Трудоемкость, чел./дн.");
    stream.writeEndElement();

    foreach (const QString& worker, workers)
    {
        stream.writeTextElement("td", QString::number(mHoursTotalWorker.value(worker).planned));

        stream.writeStartElement("td");
        if (not onlyPlan)
        {
            stream.writeCharacters(QString::number(mHoursTotalWorker.value(worker).actual));
        }
        stream.writeEndElement();
    }

    for (int i = 0; i < 2; ++i)
    {
        stream.writeTextElement("td", QString::number(mHoursTotalOverall.planned));

        stream.writeStartElement("td");
        if (not onlyPlan)
        {
            stream.writeCharacters(QString::number(mHoursTotalOverall.actual));
        }
        stream.writeEndElement();
    }

    stream.writeEndElement();

    //// Row last: workers signatures ------------------------------------------

    stream.writeStartElement("tr");

    stream.writeStartElement("td");
    stream.writeAttribute("align", "left");
    stream.writeAttribute("colspan", "3");
    stream.writeCharacters("Подпись работников");
    stream.writeEndElement();

    foreach (const QString& worker, workers)
    {
        (void)worker;

        stream.writeStartElement("td");
        stream.writeAttribute("colspan", "2");
        stream.writeEndElement();
    }

    stream.writeEndElement();

    //// -----------------------------------------------------------------------

    stream.writeEndElement();

    //// Write footer ==========================================================

    stream.writeTextElement("p", "Руководитель подразделения: ________________");
    stream.writeTextElement("p", "СОГЛАСОВАНО:");
    stream.writeTextElement("p", "Заместитель генерального директора " + sNameDivision + ": ________________");
    stream.writeTextElement("p", "Финансовый директор: ________________");
    stream.writeTextElement("p", "Исполнительный директор: ________________");

    //// =======================================================================

    stream.writeEndElement();

    file.close();
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

    //// Get date ==============================================================

    QFileInfo reportDirectoryInfo(reportDirectory.absolutePath());
    QDate reportDate;

    QString reportDirectoryName = reportDirectoryInfo.baseName();
    if (reportDirectoryName.length() == 4 + 1 + 2 &&
        reportDirectoryName[4] == '-')
    {
        int year;
        int month;

        year = reportDirectoryName.mid(0, 4).toInt();
        month = reportDirectoryName.mid(5, 2).toInt();

        reportDate.setDate(year, month, 1);
    }


    if (not reportDate.isValid())
    {
        qCritical() << "Wrong directory name";
        return 1;
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
                mHoursTotalWorker[entry.name].planned += 0;

                continue;
            }

            mHoursStat[entry.project][entry.description][entry.name].planned    += entry.hours;
            mHourstTotalProject[entry.project].planned                          += entry.hours;
            mHourstTotalTask[entry.project][entry.description].planned          += entry.hours;
            mHoursTotalWorker[entry.name].planned                               += entry.hours;
            mHoursTotalOverall.planned                                          += entry.hours;
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
            mHourstTotalTask[entry.project][entry.plan].actual       += entry.hours;
            mHoursTotalWorker[entry.name].actual                     += entry.hours;
            mHoursTotalOverall.actual                                += entry.hours;
        }
    }

    //// Load settings =========================================================

    {
        Settings settings;

        QJsonDocument settingsDocument;

        QDir settingsDir = settings.getWorkPath();

        QFile settingsFile(settingsDir.filePath("report-builder-html.json"));
        if (settingsFile.open(QIODevice::ReadOnly))
        {
            settingsDocument = QJsonDocument::fromJson(settingsFile.readAll());
        }

        //// Common ------------------------------------------------------------

        QJsonObject settingsObject = settingsDocument.object();

        QJsonObject settingsNameObject = settingsObject.value("name").toObject();
        QJsonObject settingsLeadObject = settingsObject.value("lead").toObject();

        //// Name --------------------------------------------------------------

        if (settingsNameObject.contains("department"))
        {
            sNameDepartment = settingsNameObject.value("department").toString();
        }

        if (settingsNameObject.contains("division"))
        {
            sNameDivision = settingsNameObject.value("division").toString();
        }

        //// Lead --------------------------------------------------------------

        if (settingsLeadObject.contains("default"))
        {
            sLeadDefault = settingsLeadObject.value("default").toString();
        }

        if (settingsLeadObject.contains("project") &&
            settingsLeadObject.value("project").isObject())
        {
            QJsonObject settingsLeadProjectObject = settingsLeadObject.value("project").toObject();

            foreach (const QString& key, settingsLeadProjectObject.keys())
            {
                sProjectLeads.insert(key, settingsLeadProjectObject.value(key).toString());
            }
        }
    }

    //// Write plan and report =================================================

    if (mHoursTotalOverall.actual > 0)
    {
        buildReportHtml(reportDate, reportDirectory, false);
    }
    buildReportHtml(reportDate, reportDirectory, true);

    //// =======================================================================

    return 0;
}
