#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogtaskedit.h"
#include "dialogsettingsedit.h"

#include <QDate>
#include <QMessageBox>

#include <QDebug>

#include <QDir>
#include <QFile>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QProcess>
#include <QUrl>
#include <QUrlQuery>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(on_actionTaskNew_triggered()));
    connect(ui->buttonEdit, SIGNAL(clicked()), this, SLOT(on_actionTaskEdit_triggered()));
    connect(ui->buttonRemove, SIGNAL(clicked()), this, SLOT(on_actionTaskDelete_triggered()));

    connect(ui->buttonExport, SIGNAL(clicked()), this, SLOT(exportData()));

    connect(ui->table, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(on_actionTaskEdit_triggered()));

    setupDateRange();

    //// Icons =================================================================

    ui->buttonAdd->setIcon(QIcon::fromTheme("list-add-symbolic", QIcon(":/icons/add.svg")));
    ui->buttonEdit->setIcon(QIcon::fromTheme("document-edit-symbolic", QIcon(":/icons/edit.svg")));
    ui->buttonRemove->setIcon(QIcon::fromTheme("edit-delete-symbolic", QIcon(":/icons/delete.svg")));

    ui->buttonSettings->setIcon(QIcon::fromTheme("preferences-system-symbolic", QIcon(":/icons/settings.svg")));

    ui->buttonExport->setIcon(QIcon::fromTheme("spreadsheet", QIcon(":/icons/table.svg")));
    ui->buttonSend->setIcon(QIcon::fromTheme("mail-send", QIcon(":/icons/email.svg")));

    //// =======================================================================

    loadData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDateRange()
{
    QDate currentDate = QDate::currentDate();

    QDate dateFrom = currentDate.addDays(1 - currentDate.dayOfWeek());
    QDate dateTo   = dateFrom.addDays(4);

    ui->dateFrom->setDate(dateFrom);
    ui->dateTo->setDate(dateTo);
}

void MainWindow::setItem(QTreeWidgetItem &item, const DialogTaskEdit &dialog)
{
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

        item.setData(COL_RESULT, Qt::UserRole, dialog.getTaskResult());
        item.setText(COL_RESULT, dialog.getTaskResultString());
    }
    else
    {
        item.setText(COL_PROJECT, QString());
        item.setText(COL_PRODUCT, QString());
        item.setText(COL_ACTION, QString());
        item.setText(COL_DESCRIPTION, QString());
        item.setText(COL_RESULT, QString());
    }
}

QString MainWindow::getDateRangeString() const
{
    return ui->dateFrom->date().toString("dd.MM.yyyy") + " - " + ui->dateTo->date().toString("dd.MM.yyyy");
}

void MainWindow::loadData()
{
    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        // Warning
        return;
    }

    QString taskPath(taskDir.absoluteFilePath(".task.json"));

    //// Open file =============================================================

    QFile file(taskPath);

    if (not file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QByteArray taskByteArray = file.readAll();

    //// Parse JSON ============================================================

    QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

    foreach (const QJsonValue& value, taskDocument.array())
    {
        QJsonObject object = value.toObject();

        DialogTaskEdit dialog;
        dialog.setProjectTemplates(mProjectTemplates);

        dialog.setTaskType(object["type"].toInt());
        dialog.setTaskHoursSpent(object["hours"].toInt());

        if (dialog.getTaskType() == TASK_ACTION)
        {
            dialog.setTaskProject(object["project"].toString());
            dialog.setTaskProduct(object["product"].toString());
            dialog.setTaskActionType(object["action"].toInt());
            dialog.setTaskDescription(object["description"].toString());
            dialog.setTaskResult(object["result"].toInt());
        }

        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);

        ui->table->addTopLevelItem(item);
    }

    //// =======================================================================
}

void MainWindow::saveData()
{
    //// Generate JSON =========================================================

    QJsonArray taskArray;

    const int count = ui->table->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->table->topLevelItem(i);

        QJsonObject taskObject;

        taskObject["type"]  = item->data(COL_TYPE, Qt::UserRole).toInt();
        taskObject["hours"] = item->data(COL_HOURS_SPENT, Qt::UserRole).toInt();

        if (item->data(COL_TYPE, Qt::UserRole).toInt() == TASK_ACTION)
        {
            taskObject["project"]     = item->text(COL_PROJECT);
            taskObject["product"]     = item->text(COL_PRODUCT);
            taskObject["action"]      = item->data(COL_ACTION, Qt::UserRole).toInt();
            taskObject["description"] = item->text(COL_DESCRIPTION);
            taskObject["result"]      = item->data(COL_RESULT, Qt::UserRole).toInt();
        }

        taskArray.append(taskObject);

    }

    QJsonDocument taskDocument(taskArray);

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        taskDir.mkdir(mSettings.getWorkPath());

        if (not taskDir.cd(mSettings.getWorkPath()))
        {
            //TODO: error
            return;
        }
    }

    QString taskPath(taskDir.absoluteFilePath(".task.json"));
    QString taskPathBak(taskDir.absoluteFilePath(".task.json.bak"));

    QFile::remove(taskPathBak);
    QFile::rename(taskPath, taskPathBak);

    //// Save task backup file =================================================

    QFile taskFile(taskPath);

    if (not taskFile.open(QIODevice::WriteOnly))
    {
        //TODO: error
        return;
    }

    taskFile.write(taskDocument.toJson());

    taskFile.close();

    //// =======================================================================
}

void MainWindow::on_actionTaskNew_triggered()
{
    DialogTaskEdit dialog;
    dialog.setProjectTemplates(mProjectTemplates);

    if (dialog.exec() == QDialog::Accepted)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);

        ui->table->addTopLevelItem(item);

        saveData();
    }
}

void MainWindow::on_actionTaskEdit_triggered()
{
    QTreeWidgetItem* item = ui->table->currentItem();
    if (item == Q_NULLPTR)
    {
        return;
    }

    DialogTaskEdit dialog;
    dialog.setProjectTemplates(mProjectTemplates);

    dialog.setTaskType(item->data(COL_TYPE, Qt::UserRole).toInt());
    dialog.setTaskHoursSpent(item->data(COL_HOURS_SPENT, Qt::UserRole).toInt());

    if (dialog.getTaskType() == TASK_ACTION)
    {
        dialog.setTaskProject(item->text(COL_PROJECT));
        dialog.setTaskProduct(item->text(COL_PRODUCT));
        dialog.setTaskActionType(item->data(COL_ACTION, Qt::UserRole).toInt());
        dialog.setTaskDescription(item->text(COL_DESCRIPTION));
        dialog.setTaskResult(item->data(COL_RESULT, Qt::UserRole).toInt());
    }

    if (dialog.exec() == QDialog::Accepted)
    {
        setItem(*item, dialog);

        saveData();
    }
}

void MainWindow::on_actionTaskDelete_triggered()
{
    if (QMessageBox::question(this, "Delete", "Delete selected item?") != QMessageBox::Yes)
    {
        return;
    }

    QTreeWidgetItem* item = ui->table->currentItem();
    if (item == Q_NULLPTR)
    {
        return;
    }

    delete item;

    saveData();
}

void MainWindow::on_buttonSettings_clicked()
{
    DialogSettingsEdit dialog;

    dialog.setName(mSettings.getUserName());
    dialog.setMailTo(mSettings.getMailTo());
    dialog.setReportDir(mSettings.getWorkPath());

#ifdef Q_OS_WIN
    dialog.setOutlookPath(mSettings.getOutlookPath());
#endif

    if (dialog.exec() == QDialog::Accepted)
    {
        bool save = false;

        if (mSettings.getUserName() != dialog.getName())
        {
            mSettings.setUserName(dialog.getName());
            save = true;
        }

        if (mSettings.getMailTo() != dialog.getMailTo())
        {
            mSettings.setMailTo(dialog.getMailTo());
            save = true;
        }

        if (mSettings.getWorkPath() != dialog.getReportDir())
        {
            mSettings.setWorkPath(dialog.getReportDir());
            save = true;
        }

#ifdef Q_OS_WIN
        if (mSettings.getOutlookPath() != dialog.getOutlookPath())
        {
            mSettings.setOutlookPath(dialog.getOutlookPath());
            save = true;
        }
#endif

        if (save)
        {
            mSettings.save();
        }
    }
}

void MainWindow::exportData()
{
    //// Generate CSV ==========================================================

    QString reportString;

    const int count = ui->table->topLevelItemCount();

    reportString.append("Сотрудник");
    reportString.append("\t");
    reportString.append("Диапазон");
    reportString.append("\t");

    for (int j = 0; j < COLUMN_COUNT; ++j)
    {
        reportString.append(ui->table->headerItem()->text(j));

        if (j < COLUMN_COUNT - 1)
        {
            reportString.append("\t");
        }
        else
        {
            reportString.append("\n");
        }
    }

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->table->topLevelItem(i);

        reportString.append(mSettings.getUserName());
        reportString.append("\t");
        reportString.append(getDateRangeString());
        reportString.append("\t");

        for (int j = 0; j < COLUMN_COUNT; ++j)
        {
            reportString.append(item->text(j));

            if (j < COLUMN_COUNT - 1)
            {
                reportString.append("\t");
            }
            else
            {
                reportString.append("\n");
            }
        }
    }

    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        taskDir.mkdir(mSettings.getWorkPath());

        if (not taskDir.cd(mSettings.getWorkPath()))
        {
            //TODO: error
            return;
        }
    }

    QString fileName = QString::fromUtf8("Отчет ") + getDateRangeString() + ".csv";

    QString reportPath(taskDir.absoluteFilePath(fileName));
    QString reportPathBak(taskDir.absoluteFilePath(fileName + ".bak"));

    QFile::remove(reportPathBak);
    QFile::rename(reportPath, reportPathBak);

    //// Save report file ======================================================

    QFile reportFile(reportPath);

    if (not reportFile.open(QIODevice::WriteOnly))
    {
        //TODO: error
        return;
    }

    reportFile.write(reportString.toUtf8());

    reportFile.close();
}

void MainWindow::on_buttonSend_clicked()
{
    exportData();

    //// Get report file name ==================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        //TODO: error
        return;
    }

    QString fileName = QString::fromUtf8("Отчет ") + getDateRangeString() + ".csv";
    QString filePath = taskDir.absoluteFilePath(fileName);

    //// Get program and arguments =============================================

#ifdef Q_OS_LINUX
    QUrlQuery query;
    query.addQueryItem("subject", "Отчет за неделю");
    query.addQueryItem("attach", filePath);

    QUrl url("mailto:" + mSettings.getMailTo());
    url.setQuery(query);

    QDesktopServices::openUrl(url);
#endif
#ifdef Q_OS_WIN
    QString program;
    QStringList arguments;

    program = mSettings.getOutlookPath();
    arguments << "/c" << "ipm.note" << "/m" << mSettings.getMailTo() +"&subject=Отчет за неделю" << "/a" << filePath;

    QProcess outlook;
    outlook.setProgram(program);
    outlook.setArguments(arguments);

    outlook.startDetached();

#endif
}
