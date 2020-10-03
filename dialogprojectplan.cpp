#include "dialogprojectplan.h"
#include "ui_dialogprojectplan.h"

#include <QLocale>
#include <QDate>
#include <QMessageBox>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

#include "values.h"
#include "projecttemplates.h"
#include "settings.h"
#include "dialogtaskedit.h"

#include "common/reportimport.h"

enum Column
{
    COL_TYPE,
    COL_HOURS_SPENT,
    COL_PROJECT,
    COL_PRODUCT,
    COL_ACTION,
    COL_DESCRIPTION,

    COLUMN_COUNT
};

enum ColumnReport
{
    COL_R_TYPE,
    COL_R_HOURS_PLANNED,
    COL_R_HOURS_SPENT,
    COL_R_PROJECT,
    COL_R_PRODUCT,
    COL_R_ACTION,
    COL_R_DESCRIPTION,

    COLUMN_REPORT_COUNT
};

DialogProjectPlan::DialogProjectPlan(const ProjectTemplates& projectTemplates,
                                     const Settings& settings,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProjectPlan),
    mProjectTemplates(projectTemplates),
    mSettings(settings)
{
    ui->setupUi(this);

    ui->tablePlan->header()->setStretchLastSection(false);

    ui->tablePlan->header()->setSectionResizeMode(COL_TYPE,        QHeaderView::ResizeToContents);
    ui->tablePlan->header()->setSectionResizeMode(COL_HOURS_SPENT, QHeaderView::ResizeToContents);
    ui->tablePlan->header()->setSectionResizeMode(COL_DESCRIPTION, QHeaderView::Stretch);

    //// Add months box ========================================================

    QLocale locale;

    ui->boxMonths->clear();

    for (int i = 0; i < 12; ++i)
    {
        ui->boxMonths->addItem(locale.standaloneMonthName(i + 1) +
                               " - " +
                               locale.standaloneMonthName(((i + 1) % 12) + 1));
    }

    //// Set current date ======================================================

    QDate date = QDate::currentDate();

    ui->boxMonths->setCurrentIndex(date.month() - 1);
    ui->editYear->setValue(date.year());

    //// Connect ===============================================================

    connect(ui->tablePlan, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
            this, SLOT(on_buttonEditPlan_clicked()));

    connect(ui->boxMonths, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeDate()));

    connect(ui->editYear, SIGNAL(valueChanged(int)),
            this, SLOT(changeDate()));

    connect(ui->widgetWorkDays, SIGNAL(daysUpdated()), this, SLOT(updatePlanHours()));

    connect(ui->widgetWorkDays, SIGNAL(daysUpdated()), this, SLOT(savePlan()));

    //// Load plan =============================================================

    changeDate();
}

DialogProjectPlan::~DialogProjectPlan()
{
    delete ui;
}

void DialogProjectPlan::on_buttonAddPlan_clicked()
{
    DialogTaskEdit dialog;
    dialog.setProjectTemplates(mProjectTemplates);
    dialog.setPlanMode(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);

        QUuid uuid = QUuid::createUuid();
        item->setData(0, Qt::UserRole + 1, uuid.toString());

        ui->tablePlan->addTopLevelItem(item);

        updatePlanHours();

        savePlan();
    }
}

void DialogProjectPlan::on_buttonEditPlan_clicked()
{
    QList<QTreeWidgetItem*> items = ui->tablePlan->selectedItems();
    if (items.isEmpty())
    {
        return;
    }

    QTreeWidgetItem* item = items.first();
    if (item == Q_NULLPTR)
    {
        return;
    }

    DialogTaskEdit dialog;
    dialog.setProjectTemplates(mProjectTemplates);
    dialog.setPlanMode(true);

    dialog.setTaskType(item->data(COL_TYPE, Qt::UserRole).toInt());
    dialog.setTaskHoursSpent(item->data(COL_HOURS_SPENT, Qt::UserRole).toInt());

    if (dialog.getTaskType() == TASK_ACTION)
    {
        dialog.setTaskProject(item->text(COL_PROJECT));
        dialog.setTaskProduct(item->text(COL_PRODUCT));
        dialog.setTaskActionType(item->data(COL_ACTION, Qt::UserRole).toInt());
        dialog.setTaskDescription(item->text(COL_DESCRIPTION));
    }

    if (dialog.exec() == QDialog::Accepted)
    {
        setItem(*item, dialog);

        updatePlanHours();

        savePlan();
    }
}

void DialogProjectPlan::on_buttonRemovePlan_clicked()
{
    QList<QTreeWidgetItem*> items = ui->tablePlan->selectedItems();
    if (items.isEmpty())
    {
        return;
    }

    if (QMessageBox::question(this, "Delete", "Delete selected item?") != QMessageBox::Yes)
    {
        return;
    }

    foreach (QTreeWidgetItem* item, items)
    {
        delete item;
    }

    updatePlanHours();

    savePlan();
}

void DialogProjectPlan::updatePlanDate(const QDate& date)
{
    //// Update work days widget ===============================================

    ui->widgetWorkDays->setDate(date);

    //// Update plan hours =====================================================

    updatePlanHours();
}

void DialogProjectPlan::updatePlanHours()
{
    int hoursWork = 0;
    int hoursTotal = ui->widgetWorkDays->workDaysCount() * 8;

    const int count = ui->tablePlan->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->tablePlan->topLevelItem(i);

        hoursWork += item->data(COL_HOURS_SPENT, Qt::UserRole).toInt();
    }

    ui->labelPlanHours->setText(QString::asprintf("%d / %d", hoursWork, hoursTotal));

    QPalette labelPalette = this->palette();

    if (hoursWork == hoursTotal)
    {
        labelPalette.setColor(QPalette::WindowText, QColor(56, 142, 60));
    }
    else
    {
        labelPalette.setColor(QPalette::WindowText, QColor(183, 28, 28));
    }

    ui->labelPlanHours->setPalette(labelPalette);
}

void DialogProjectPlan::changeDate()
{
    QDate date;
    date.setDate(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

    QDate dateNext = date.addMonths(1);

    //// Set titles ============================================================

    QLocale locale;

    ui->tabWidget->setTabText(0, QString::fromUtf8("Отчет за ") +
                              locale.standaloneMonthName(date.month(), QLocale::LongFormat) +
                              " " + QString::number(date.year()) +
                              QString::fromUtf8(" г."));

    ui->tabWidget->setTabText(1, QString::fromUtf8("План на ") +
                              locale.standaloneMonthName(dateNext.month(), QLocale::LongFormat) +
                              " " + QString::number(dateNext.year()) +
                              QString::fromUtf8(" г."));

    //// Reload Plan ===========================================================

    loadPlan();

    //// Reload month report ===================================================

    loadMonthReport();
}

void DialogProjectPlan::loadPlan()
{
    QDate date;
    date.setDate(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

    QDate dateNext = date.addMonths(1);

    //// Clear data ============================================================

    ui->tablePlan->clear();

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        // Warning
        updatePlanDate(dateNext);
        return;
    }

    if (not taskDir.cd(".plan"))
    {
        // Warning
        updatePlanDate(dateNext);
        return;
    }

    QString fileName = dateNext.toString("yyyy-MM") +".json";

    QString taskPath(taskDir.absoluteFilePath(fileName));

    //// Open file =============================================================

    QFile file(taskPath);

    if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        updatePlanDate(dateNext);
        return;
    }

    QByteArray taskByteArray = file.readAll();

    //// Load JSON =============================================================

    QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

    QJsonObject reportObject = taskDocument.object();

    //// Add tasks =============================================================

    foreach (const QJsonValue& value, reportObject["tasks"].toArray())
    {
        QJsonObject object = value.toObject();

        DialogTaskEdit dialog;
        dialog.setProjectTemplates(mProjectTemplates);
        dialog.setPlanMode(true);

        //// Get type ----------------------------------------------------------

        QByteArray typeStr = object["type"].toString().toUtf8();

        for (int i = 0; i < TASK_COUNT; ++i)
        {
            if (QByteArray(gValuesTaskTypes[i].jsonValue) == typeStr)
            {
                dialog.setTaskType(gValuesTaskTypes[i].value);
            }
        }

        //// Get hours ---------------------------------------------------------

        dialog.setTaskHoursSpent(object["hours"].toInt());

        //// Parse action ------------------------------------------------------

        if (dialog.getTaskType() == TASK_ACTION)
        {
            //// Get string values ---------------------------------------------

            dialog.setTaskProject(object["project"].toString());
            dialog.setTaskProduct(object["product"].toString());
            dialog.setTaskDescription(object["description"].toString());

            //// Get action ----------------------------------------------------

            QByteArray actionStr = object["action"].toString().toUtf8();

            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                if (QByteArray(gValuesActionTypes[i].jsonValue) == actionStr)
                {
                    dialog.setTaskActionType(gValuesActionTypes[i].value);
                }
            }

            //// ---------------------------------------------------------------
        }

        //// Get UUID ----------------------------------------------------------

        QString uuidStr;

        if (object.contains("uuid"))
        {
            uuidStr = object["uuid"].toString();

            if (QUuid(uuidStr.toLatin1()).isNull())
            {
                uuidStr = QUuid::createUuid().toString();
            }
        }

        //// Add item ----------------------------------------------------------

        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);
        item->setData(0, Qt::UserRole + 1, uuidStr);

        ui->tablePlan->addTopLevelItem(item);
    }

    //// Update date ===========================================================

    updatePlanDate(dateNext);

    //// Load work days list ===================================================

    if (reportObject["workDays"].isArray())
    {
        QList<int> workDaysList;

        foreach (const QJsonValue& value, reportObject["workDays"].toArray())
        {
            workDaysList.append(value.toInt());
        }

        ui->widgetWorkDays->setWorkDays(workDaysList);
    }

    //// Update plan hours =====================================================

    updatePlanHours();

    //// =======================================================================
}

void DialogProjectPlan::loadMonthReport()
{
    QDate date;
    date.setDate(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

    //// Clear data ============================================================

    ui->tableMonthReport->clear();

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        // Warning
        return;
    }

    if (not taskDir.cd(".plan"))
    {
        // Warning
        return;
    }

    QString fileName = date.toString("yyyy-MM") +".json";

    QString taskPath(taskDir.absoluteFilePath(fileName));

    //// Open file =============================================================

    QFile file(taskPath);

    if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray taskByteArray = file.readAll();

    //// Load JSON =============================================================

    QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

    QJsonObject reportObject = taskDocument.object();

    //// Add tasks =============================================================

    foreach (const QJsonValue& value, reportObject["tasks"].toArray())
    {
        QJsonObject object = value.toObject();

        DialogTaskEdit dialog;
        dialog.setProjectTemplates(mProjectTemplates);
        dialog.setPlanMode(true);

        //// Get type ----------------------------------------------------------

        QByteArray typeStr = object["type"].toString().toUtf8();

        for (int i = 0; i < TASK_COUNT; ++i)
        {
            if (QByteArray(gValuesTaskTypes[i].jsonValue) == typeStr)
            {
                dialog.setTaskType(gValuesTaskTypes[i].value);
            }
        }

        //// Get hours ---------------------------------------------------------

        dialog.setTaskHoursSpent(object["hours"].toInt());

        //// Parse action ------------------------------------------------------

        if (dialog.getTaskType() == TASK_ACTION)
        {
            //// Get string values ---------------------------------------------

            dialog.setTaskProject(object["project"].toString());
            dialog.setTaskProduct(object["product"].toString());
            dialog.setTaskDescription(object["description"].toString());

            //// Get action ----------------------------------------------------

            QByteArray actionStr = object["action"].toString().toUtf8();

            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                if (QByteArray(gValuesActionTypes[i].jsonValue) == actionStr)
                {
                    dialog.setTaskActionType(gValuesActionTypes[i].value);
                }
            }

            //// ---------------------------------------------------------------
        }

        //// Get UUID ----------------------------------------------------------

        QString uuidStr;

        if (object.contains("uuid"))
        {
            uuidStr = object["uuid"].toString();

            if (QUuid(uuidStr.toLatin1()).isNull())
            {
                uuidStr = QUuid::createUuid().toString();
            }
        }

        //// Add item ----------------------------------------------------------

        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog, true);
        item->setData(0, Qt::UserRole + 1, uuidStr);
        item->setData(0, Qt::UserRole + 2, true);

        ui->tableMonthReport->addTopLevelItem(item);

        //// Set bold font -----------------------------------------------------

        QFont fontBold = ui->tableMonthReport->font();
        fontBold.setBold(true);
        for (int i = 0; i < ui->tableMonthReport->columnCount(); ++i)
        {
            item->setFont(i, fontBold);
        }
    }

    //// Add unplanned tasks ===================================================

    foreach (const QJsonValue& value, reportObject["unplanned"].toArray())
    {
        QJsonObject object = value.toObject();

        DialogTaskEdit dialog;
        dialog.setProjectTemplates(mProjectTemplates);
        dialog.setPlanMode(true);

        //// Get type ----------------------------------------------------------

        QByteArray typeStr = object["type"].toString().toUtf8();

        for (int i = 0; i < TASK_COUNT; ++i)
        {
            if (QByteArray(gValuesTaskTypes[i].jsonValue) == typeStr)
            {
                dialog.setTaskType(gValuesTaskTypes[i].value);
            }
        }

        //// Get hours ---------------------------------------------------------

        //dialog.setTaskHoursSpent(object["hours"].toInt());

        //// Parse action ------------------------------------------------------

        if (dialog.getTaskType() == TASK_ACTION)
        {
            //// Get string values ---------------------------------------------

            dialog.setTaskProject(object["project"].toString());
            dialog.setTaskProduct(object["product"].toString());
            dialog.setTaskDescription(object["description"].toString());

            //// Get action ----------------------------------------------------

            QByteArray actionStr = object["action"].toString().toUtf8();

            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                if (QByteArray(gValuesActionTypes[i].jsonValue) == actionStr)
                {
                    dialog.setTaskActionType(gValuesActionTypes[i].value);
                }
            }

            //// ---------------------------------------------------------------
        }

        //// Get UUID ----------------------------------------------------------

        QString uuidStr;

        if (object.contains("uuid"))
        {
            uuidStr = object["uuid"].toString();

            if (QUuid(uuidStr.toLatin1()).isNull())
            {
                uuidStr = QUuid::createUuid().toString();
            }
        }

        //// Add item ----------------------------------------------------------

        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog, true);
        item->setData(COL_HOURS_SPENT, Qt::UserRole, 0);
        item->setText(COL_HOURS_SPENT, "0");
        item->setData(0, Qt::UserRole + 1, uuidStr);
        item->setData(0, Qt::UserRole + 2, false);

        QFont fontBold = ui->tableMonthReport->font();
        fontBold.setBold(true);
        for (int i = 1; i < ui->tableMonthReport->columnCount(); ++i)
        {
            item->setFont(i, fontBold);
        }

        ui->tableMonthReport->addTopLevelItem(item);

    }

    //// Load tasks ============================================================

    loadMonthTasks();

    //// =======================================================================
}

void DialogProjectPlan::loadMonthTasks()
{
    QDate date;
    date.setDate(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        // Warning
        return;
    }

    QString dirName = date.toString("yyyy-MM");

    if (not taskDir.cd(dirName))
    {
        // Warning
        return;
    }

    QStringList filters;
    filters << "*.csv";

    //// Load entries ==========================================================

    foreach (const QFileInfo& d, taskDir.entryInfoList(filters, QDir::Files))
    {
        ReportImport importer;
        QList<ReportEntry> entries;

        if (not importer.readReport(d.absoluteFilePath(), entries))
        {
            QString error = importer.lastError();

            if (importer.lastErrorLine() > -1)
            {
                error.append(QString::asprintf("\nat line %d",
                                               importer.lastErrorLine()));
            }

            QMessageBox::critical(this,
                                  QString::fromUtf8("Импорт"),
                                  error);

            return;
        }

        foreach (const ReportEntry& report, entries)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem;

            //// Finn report data ----------------------------------------------

            item->setIcon(COL_R_TYPE, QIcon::fromTheme("text", QIcon(":/icons/text.svg")));

            item->setData(COL_R_TYPE, Qt::UserRole, report.type);
            item->setText(COL_R_TYPE, QString::fromUtf8(gValuesTaskTypes[report.type].displayValue));

            item->setData(COL_R_HOURS_SPENT, Qt::UserRole, report.hours);
            item->setText(COL_R_HOURS_SPENT, QString::number(report.hours));

            if (report.type == TASK_ACTION)
            {
                item->setText(COL_R_PROJECT, report.project);

                item->setText(COL_R_PRODUCT, report.product);

                item->setData(COL_R_ACTION, Qt::UserRole, report.action);
                item->setText(COL_R_ACTION, QString::fromUtf8(gValuesActionTypes[report.action].displayValue));

                item->setText(COL_R_DESCRIPTION, report.description);
            }
            else
            {
                item->setText(COL_R_PROJECT, QString());
                item->setText(COL_R_PRODUCT, QString());
                item->setText(COL_R_ACTION, QString());
                item->setText(COL_R_DESCRIPTION, QString());
            }

            //// Match with planned task ---------------------------------------

            int count = ui->tableMonthReport->topLevelItemCount();
            QTreeWidgetItem* rootItem = nullptr; //ui->tableMonthReport->topLevelItem(count - 1);

            for (int i = 0; i < count - 1; ++i)
            {
                QTreeWidgetItem* check = ui->tableMonthReport->topLevelItem(i);

                if (item->text(COL_R_ACTION) != check->text(COL_R_ACTION))
                {
                    continue;
                }

                if (item->text(COL_R_PROJECT) != check->text(COL_R_PROJECT))
                {
                    continue;
                }

                if (item->text(COL_R_PRODUCT) != check->text(COL_R_PRODUCT))
                {
                    continue;
                }

                if (report.plan != check->text(COL_R_DESCRIPTION))
                {
                    continue;
                }

                rootItem = check;
            }

            //// Create new unplanned task if not matched ======================

            if (rootItem == nullptr)
            {
                rootItem = new QTreeWidgetItem(*item);

                rootItem->setData(COL_R_HOURS_PLANNED, Qt::UserRole, 0);
                rootItem->setText(COL_R_HOURS_PLANNED, "-");

                rootItem->setText(COL_R_DESCRIPTION, report.plan);

                QFont fontBold = ui->tableMonthReport->font();
                fontBold.setBold(true);
                for (int i = 1; i < ui->tableMonthReport->columnCount(); ++i)
                {
                    rootItem->setFont(i, fontBold);
                }

                ui->tableMonthReport->addTopLevelItem(rootItem);
            }

            //// Add item ------------------------------------------------------

            if (rootItem != nullptr)
            {
                rootItem->addChild(item);
            }
        }
    }

    //// Update hours ==========================================================

    updateMonthReportHours();
}

void DialogProjectPlan::updateMonthReportHours()
{
    const QColor colorMatched   = QColor(56, 142, 60);
    const QColor colorUnmatched = QColor(183, 28, 28);

    int hoursPlanned = 0;
    int hoursSpent = 0;

    int count = ui->tableMonthReport->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        QTreeWidgetItem* rootItem = ui->tableMonthReport->topLevelItem(i);

        hoursPlanned += rootItem->data(COL_R_HOURS_PLANNED, Qt::UserRole).toInt();

        int taskCount = rootItem->childCount();

        int hoursSpentPlan = 0;

        for (int j = 0; j < taskCount; ++j)
        {
            QTreeWidgetItem* item = rootItem->child(j);

            hoursSpentPlan += item->data(COL_R_HOURS_SPENT, Qt::UserRole).toInt();
        }

        rootItem->setData(COL_R_HOURS_SPENT, Qt::UserRole, hoursSpentPlan);
        rootItem->setText(COL_R_HOURS_SPENT, QString::number(hoursSpentPlan));

        if (hoursSpentPlan >= rootItem->data(COL_R_HOURS_PLANNED, Qt::UserRole).toInt())
        {
            rootItem->setTextColor(COL_R_HOURS_SPENT, colorMatched);
        }
        else
        {
            rootItem->setTextColor(COL_R_HOURS_SPENT, colorUnmatched);
        }

        hoursSpent += hoursSpentPlan;
    }

    ui->labelReportHours->setText(QString::asprintf("%d / %d", hoursSpent, hoursPlanned));

    QPalette labelPalette = this->palette();

    if (hoursSpent == hoursPlanned)
    {
        labelPalette.setColor(QPalette::WindowText, colorMatched);
    }
    else
    {
        labelPalette.setColor(QPalette::WindowText, colorUnmatched);
    }

    ui->labelReportHours->setPalette(labelPalette);
}

void DialogProjectPlan::savePlan()
{
    QDate date;
    date.setDate(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

    QDate dateNext = date.addMonths(1);

    //// Generate JSON =========================================================

    QJsonObject reportObject;

    QJsonArray taskArray;

    //// Planned items ---------------------------------------------------------

    const int count = ui->tablePlan->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->tablePlan->topLevelItem(i);

        QJsonObject taskObject;

        int taskId = item->data(COL_TYPE, Qt::UserRole).toInt();
        if (taskId >= TASK_COUNT)
        {
            //TODO: error
            taskId = 0;
        }

        taskObject["type"]  = QString::fromUtf8(gValuesTaskTypes[taskId].jsonValue);
        taskObject["hours"] = item->data(COL_HOURS_SPENT, Qt::UserRole).toInt();

        if (item->data(COL_TYPE, Qt::UserRole).toInt() == TASK_ACTION)
        {
            int actionId = item->data(COL_ACTION, Qt::UserRole).toInt();
            if (actionId >= ACTION_COUNT)
            {
                //TODO: error
                actionId = 0;
            }

            taskObject["project"]     = item->text(COL_PROJECT);
            taskObject["product"]     = item->text(COL_PRODUCT);
            taskObject["action"]      = QString::fromUtf8(gValuesActionTypes[actionId].jsonValue);
            taskObject["description"] = item->text(COL_DESCRIPTION);

            taskObject["uuid"]        = item->data(0, Qt::UserRole + 1).toString();
        }

        taskArray.append(taskObject);

    }

    reportObject["tasks"] = taskArray;

    //// Add work days list ----------------------------------------------------

    QJsonArray workDaysArray;

    foreach (int workDay, ui->widgetWorkDays->workDays())
    {
        workDaysArray.append(workDay);
    }

    reportObject["workDays"] = workDaysArray;

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        taskDir.mkdir(mSettings.getWorkPath());

        if (not taskDir.cd(mSettings.getWorkPath()))
        {
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать директорию для отчетов"));
            return;
        }
    }

    QString planDir = ".plan";

    if (not taskDir.cd(planDir))
    {
        taskDir.mkdir(planDir);

        if (not taskDir.cd(planDir))
        {
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать директорию для планов"));
            return;
        }
    }

    QString fileName = dateNext.toString("yyyy-MM") +".json";

    QString taskPath(taskDir.absoluteFilePath(fileName));
    QString taskPathBak(taskDir.absoluteFilePath(fileName + ".bak"));

    QFile::remove(taskPathBak);
    QFile::rename(taskPath, taskPathBak);

    //// Save task backup file =================================================

    QFile taskFile(taskPath);

    if (not taskFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно записать файл отчета"));
        return;
    }

    QJsonDocument reportDocument(reportObject);

    taskFile.write(reportDocument.toJson());

    taskFile.close();

    //// Make plan directory hidden (for Windows) ==============================

#ifdef Q_OS_WIN
    SetFileAttributes(reinterpret_cast<LPCWSTR>(taskDir.absolutePath().utf16()), FILE_ATTRIBUTE_HIDDEN);
#endif

    //// Remove backup file ====================================================

    QFile::remove(taskPathBak);

    //// =======================================================================
}

void DialogProjectPlan::saveMonthReport()
{
    QDate date;
    date.setDate(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        taskDir.mkdir(mSettings.getWorkPath());

        if (not taskDir.cd(mSettings.getWorkPath()))
        {
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать директорию для отчетов"));
            return;
        }
    }

    QString planDir = ".plan";

    if (not taskDir.cd(planDir))
    {
        taskDir.mkdir(planDir);

        if (not taskDir.cd(planDir))
        {
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать директорию для планов"));
            return;
        }
    }

    QString fileName = date.toString("yyyy-MM") +".json";

    QString taskPath(taskDir.absoluteFilePath(fileName));
    QString taskPathBak(taskDir.absoluteFilePath(fileName + ".bak"));

    //// Open file =============================================================

    QFile file(taskPath);

    if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray taskByteArray = file.readAll();

    //// Load JSON =============================================================

    QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

    QJsonObject reportObject = taskDocument.object();

    //// Add unplanned tesks ===================================================

    QJsonArray taskArray;

    //// Planned items ---------------------------------------------------------

    const int count = ui->tableMonthReport->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->tableMonthReport->topLevelItem(i);

        bool readonly = item->data(0, Qt::UserRole + 2).toBool();
        if (readonly)
        {
            continue;
        }

        QJsonObject taskObject;

        int taskId = item->data(COL_R_TYPE, Qt::UserRole).toInt();
        if (taskId >= TASK_COUNT)
        {
            //TODO: error
            taskId = 0;
        }

        taskObject["type"]  = QString::fromUtf8(gValuesTaskTypes[taskId].jsonValue);
//        taskObject["hours"] = item->data(COL_R_HOURS_SPENT, Qt::UserRole).toInt();

        if (item->data(COL_R_TYPE, Qt::UserRole).toInt() == TASK_ACTION)
        {
            int actionId = item->data(COL_R_ACTION, Qt::UserRole).toInt();
            if (actionId >= ACTION_COUNT)
            {
                //TODO: error
                actionId = 0;
            }

            taskObject["project"]     = item->text(COL_R_PROJECT);
            taskObject["product"]     = item->text(COL_R_PRODUCT);
            taskObject["action"]      = QString::fromUtf8(gValuesActionTypes[actionId].jsonValue);
            taskObject["description"] = item->text(COL_R_DESCRIPTION);

            taskObject["uuid"]        = item->data(0, Qt::UserRole + 1).toString();
        }

        taskArray.append(taskObject);

    }

    reportObject["unplanned"] = taskArray;

    //// =======================================================================

    taskDocument = QJsonDocument(reportObject);

    //// Create backup file ====================================================

    QFile::remove(taskPathBak);
    QFile::rename(taskPath, taskPathBak);

    //// Save task backup file =================================================

    QFile taskFile(taskPath);

    if (not taskFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно записать файл отчета"));
        return;
    }

    QJsonDocument reportDocument(reportObject);

    taskFile.write(reportDocument.toJson());

    taskFile.close();

    //// Make plan directory hidden (for Windows) ==============================

#ifdef Q_OS_WIN
    SetFileAttributes(reinterpret_cast<LPCWSTR>(taskDir.absolutePath().utf16()), FILE_ATTRIBUTE_HIDDEN);
#endif

    //// Remove backup file ====================================================

    QFile::remove(taskPathBak);

    //// =======================================================================
}

void DialogProjectPlan::exportPlan()
{

}

void DialogProjectPlan::setItem(QTreeWidgetItem& item, const DialogTaskEdit& dialog, bool isReport)
{
    int col_type        = isReport ? (int)COL_R_TYPE          : (int)COL_TYPE;
    int col_hours       = isReport ? (int)COL_R_HOURS_PLANNED : (int)COL_HOURS_SPENT;
    int col_project     = isReport ? (int)COL_R_PROJECT       : (int)COL_PROJECT;
    int col_product     = isReport ? (int)COL_R_PRODUCT       : (int)COL_PRODUCT;
    int col_action      = isReport ? (int)COL_R_ACTION        : (int)COL_ACTION;
    int col_description = isReport ? (int)COL_R_DESCRIPTION   : (int)COL_DESCRIPTION;

    item.setIcon(col_type, QIcon::fromTheme("text", QIcon(":/icons/text.svg")));

    item.setData(col_type, Qt::UserRole, dialog.getTaskType());
    item.setText(col_type, dialog.getTaskTypeString());

    item.setData(col_hours, Qt::UserRole, dialog.getTaskHoursSpent());
    item.setText(col_hours, QString::number(dialog.getTaskHoursSpent()));

    if (dialog.getTaskType() == TASK_ACTION)
    {
        item.setText(col_project, dialog.getTaskProject());

        item.setText(col_product, dialog.getTaskProduct());

        item.setData(col_action, Qt::UserRole, dialog.getTaskActionType());
        item.setText(col_action, dialog.getTaskActionTypeString());

        item.setText(col_description, dialog.getTaskDescription());
    }
    else
    {
        item.setText(col_project,     QString());
        item.setText(col_product,     QString());
        item.setText(col_action,      QString());
        item.setText(col_description, QString());
    }
}

void DialogProjectPlan::on_buttonMoveBack_clicked()
{
    moveInTime(-1);
}

void DialogProjectPlan::on_buttonMoveForward_clicked()
{
    moveInTime(1);
}

void DialogProjectPlan::moveInTime(int months)
{
    QDate date(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

    date = date.addMonths(months);

    ui->editYear->setValue(date.year());
    ui->boxMonths->setCurrentIndex(date.month() - 1);
}

void DialogProjectPlan::on_buttonGoToday_clicked()
{
    QDate date = QDate::currentDate();

    ui->editYear->setValue(date.year());
    ui->boxMonths->setCurrentIndex(date.month() - 1);
}
