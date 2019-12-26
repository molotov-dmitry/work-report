#include "dialogprojectplan.h"
#include "ui_dialogprojectplan.h"

#include <QLocale>
#include <QDate>
#include <QMessageBox>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "values.h"
#include "projecttemplates.h"
#include "settings.h"
#include "dialogtaskedit.h"

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

DialogProjectPlan::DialogProjectPlan(const ProjectTemplates& projectTemplates,
                                     const Settings& settings,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProjectPlan),
    mProjectTemplates(projectTemplates),
    mSettings(settings),
    mPlanChanged(false)
{
    ui->setupUi(this);

    ui->tablePlan->header()->setStretchLastSection(false);

    ui->tablePlan->header()->setSectionResizeMode(COL_TYPE,        QHeaderView::ResizeToContents);
    ui->tablePlan->header()->setSectionResizeMode(COL_HOURS_SPENT, QHeaderView::ResizeToContents);
    ui->tablePlan->header()->setSectionResizeMode(COL_DESCRIPTION, QHeaderView::Stretch);

    //// Add months box ========================================================

    QLocale locale;

    ui->boxMonths->clear();

    for (int i = 1; i <= 12; ++i)
    {
        ui->boxMonths->addItem(locale.standaloneMonthName(i));
    }

    //// Set current date ======================================================

    QDate date = QDate::currentDate();
    date = date.addMonths(1);

    ui->boxMonths->setCurrentIndex(date.month() - 1);
    ui->editYear->setValue(date.year());

    //// Connect ===============================================================

    connect(ui->boxMonths, SIGNAL(currentIndexChanged(int)),
            this, SLOT(loadPlan()));

    connect(ui->editYear, SIGNAL(valueChanged(int)),
            this, SLOT(loadPlan()));

    //// Load plan =============================================================

    loadPlan();
}

DialogProjectPlan::~DialogProjectPlan()
{
    delete ui;
}

void DialogProjectPlan::on_buttonAdd_clicked()
{
    DialogTaskEdit dialog;
    dialog.setProjectTemplates(mProjectTemplates);
    dialog.setPlanMode(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);

        ui->tablePlan->addTopLevelItem(item);

        updatePlanHours();

        mPlanChanged = true;
    }
}

void DialogProjectPlan::on_buttonEdit_clicked()
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

        mPlanChanged = true;
    }
}

void DialogProjectPlan::on_buttonRemove_clicked()
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

    mPlanChanged = true;
}

void DialogProjectPlan::updatePlanHours()
{
    int hoursWork = 0;
//    int hoursTotal = (ui->dateFrom->date().daysTo(ui->dateTo->date()) + 1) * 8;

    const int count = ui->tablePlan->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->tablePlan->topLevelItem(i);

        hoursWork += item->data(COL_HOURS_SPENT, Qt::UserRole).toInt();
    }

    ui->labelPlanHours->setText(QString::asprintf("%d", hoursWork));

//    ui->labelPlanHours->setText(QString::asprintf("%d / %d", hoursWork, hoursTotal));

//    QPalette labelPalette = this->palette();

//    if (hoursWork == hoursTotal)
//    {
//        labelPalette.setColor(QPalette::WindowText, QColor(56, 142, 60));
//    }
//    else
//    {
//        labelPalette.setColor(QPalette::WindowText, QColor(183, 28, 28));
//    }

//    ui->labelTotalHours->setPalette(labelPalette);
}

void DialogProjectPlan::loadPlan()
{
    ui->tablePlan->clear();
    mPlanChanged = false;

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

    QDate date;
    date.setDate(ui->editYear->value(), ui->boxMonths->currentIndex() + 1, 1);

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

        //// Add item ----------------------------------------------------------

        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);

        ui->tablePlan->addTopLevelItem(item);
    }

    //// Update plan hours =====================================================

    updatePlanHours();

    //// =======================================================================
}

void DialogProjectPlan::savePlan()
{
    //// Generate JSON =========================================================

    QJsonObject reportObject;

    QJsonArray taskArray;

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
        }

        taskArray.append(taskObject);

    }

    reportObject["tasks"] = taskArray;

    QJsonDocument reportDocument(reportObject);

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        taskDir.mkdir(mSettings.getWorkPath());

        if (not taskDir.cd(mSettings.getWorkPath()))
        {
            QMessageBox::critical(this, QString::fromUtf8("����������"), QString::fromUtf8("���������� ������� ���������� ��� �������"));
            return;
        }
    }

    QString planDir = ".plan";

    if (not taskDir.cd(planDir))
    {
        taskDir.mkdir(planDir);

        if (not taskDir.cd(planDir))
        {
            QMessageBox::critical(this, QString::fromUtf8("����������"), QString::fromUtf8("���������� ������� ���������� ��� ������"));
            return;
        }
    }

    QString fileName = QDate::currentDate().toString("yyyy-MM") +".json";

    QString taskPath(taskDir.absoluteFilePath(fileName));
    QString taskPathBak(taskDir.absoluteFilePath(fileName + ".bak"));

    QFile::remove(taskPathBak);
    QFile::rename(taskPath, taskPathBak);

    //// Save task backup file =================================================

    QFile taskFile(taskPath);

    if (not taskFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, QString::fromUtf8("����������"), QString::fromUtf8("���������� �������� ���� ������"));
        return;
    }

    taskFile.write(reportDocument.toJson());

    taskFile.close();

    //// Make file hidden (for Windows) ========================================

#ifdef Q_OS_WIN
    SetFileAttributes(reinterpret_cast<LPCWSTR>(taskPath.utf16()), FILE_ATTRIBUTE_HIDDEN);
#endif

    //// Remove backup file ====================================================

    QFile::remove(taskPathBak);

    //// =======================================================================

    mPlanChanged = false;
}

void DialogProjectPlan::exportPlan()
{

}

void DialogProjectPlan::setItem(QTreeWidgetItem& item, const DialogTaskEdit& dialog)
{
    item.setIcon(COL_TYPE, QIcon::fromTheme("text", QIcon(":/icons/text.svg")));

    item.setData(COL_TYPE, Qt::UserRole, dialog.getTaskType());
    item.setText(COL_TYPE, dialog.getTaskTypeString());

    item.setData(COL_HOURS_SPENT, Qt::UserRole, dialog.getTaskHoursSpent());
    item.setText(COL_HOURS_SPENT, QString::number(dialog.getTaskHoursSpent()));

    if (dialog.getTaskType() == TASK_ACTION)
    {
        item.setText(COL_PROJECT, dialog.getTaskProject());

        item.setText(COL_PRODUCT, dialog.getTaskProduct());

        item.setData(COL_ACTION, Qt::UserRole, dialog.getTaskActionType());
        item.setText(COL_ACTION, dialog.getTaskActionTypeString());

        item.setText(COL_DESCRIPTION, dialog.getTaskDescription());
    }
    else
    {
        item.setText(COL_PROJECT, QString());
        item.setText(COL_PRODUCT, QString());
        item.setText(COL_ACTION, QString());
        item.setText(COL_DESCRIPTION, QString());
    }
}
