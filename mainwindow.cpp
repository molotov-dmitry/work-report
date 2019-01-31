#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogtaskedit.h"
#include "dialogsettingsedit.h"
#include "dialogprojecttemplatesedit.h"

#include <QShortcut>

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

#include "values.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mDataExported(false)
{
    ui->setupUi(this);

    connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(on_actionTaskNew_triggered()));
    connect(ui->buttonEdit, SIGNAL(clicked()), this, SLOT(on_actionTaskEdit_triggered()));
    connect(ui->buttonRemove, SIGNAL(clicked()), this, SLOT(on_actionTaskDelete_triggered()));

    connect(ui->buttonExport, SIGNAL(clicked()), this, SLOT(exportData()));

    connect(ui->table, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(on_actionTaskEdit_triggered()));

    connect(ui->dateFrom, SIGNAL(userDateChanged(const QDate &)), this, SLOT(dateRangeChanged()));
    connect(ui->dateTo,   SIGNAL(userDateChanged(const QDate &)), this, SLOT(dateRangeChanged()));

    //// Shortcuts =============================================================

    QShortcut *shortcutAdd1 = new QShortcut(Qt::Key_Insert, this);
    QObject::connect(shortcutAdd1, SIGNAL(activated()), this, SLOT(on_actionTaskNew_triggered()));

    QShortcut *shortcutAdd2 = new QShortcut(Qt::Key_Plus, this);
    QObject::connect(shortcutAdd2, SIGNAL(activated()), this, SLOT(on_actionTaskNew_triggered()));

    QShortcut *shortcutEdit1 = new QShortcut(Qt::CTRL + 'E', this);
    QObject::connect(shortcutEdit1, SIGNAL(activated()), this, SLOT(on_actionTaskEdit_triggered()));

    QShortcut *shortcutEdit2 = new QShortcut(Qt::Key_Space, this);
    QObject::connect(shortcutEdit2, SIGNAL(activated()), this, SLOT(on_actionTaskEdit_triggered()));

    QShortcut *shortcutDelete1 = new QShortcut(Qt::Key_Delete, this);
    QObject::connect(shortcutDelete1, SIGNAL(activated()), this, SLOT(on_actionTaskDelete_triggered()));

    QShortcut *shortcutDelete2 = new QShortcut(Qt::Key_Minus, this);
    QObject::connect(shortcutDelete2, SIGNAL(activated()), this, SLOT(on_actionTaskDelete_triggered()));

    //// Icons =================================================================

    this->setWindowIcon(QIcon::fromTheme("time-admin", QIcon(":/icons/clock.svg")));

    ui->buttonAdd->setIcon(QIcon::fromTheme("list-add-symbolic", QIcon(":/icons/add.svg")));
    ui->buttonEdit->setIcon(QIcon::fromTheme("document-edit-symbolic", QIcon(":/icons/edit.svg")));
    ui->buttonRemove->setIcon(QIcon::fromTheme("edit-delete-symbolic", QIcon(":/icons/delete.svg")));

    ui->buttonSettings->setIcon(QIcon::fromTheme("preferences-system-symbolic", QIcon(":/icons/settings.svg")));
    ui->buttonTemplates->setIcon(QIcon::fromTheme("folder-templates-symbolic", QIcon(":/icons/template.svg")));

    ui->buttonOpenReportDir->setIcon(QIcon::fromTheme("folder", QIcon(":/icons/folder.svg")));

    ui->buttonExport->setIcon(QIcon::fromTheme("spreadsheet", QIcon(":/icons/table.svg")));
    ui->buttonSend->setIcon(QIcon::fromTheme("mail-send", QIcon(":/icons/email.svg")));

    //// =======================================================================

    setupDateRange();

    loadData();
    updateTotalHours();

    //// Setup table ===========================================================

    ui->table->header()->setStretchLastSection(false);

    ui->table->header()->setSectionResizeMode(COL_HOURS_SPENT, QHeaderView::ResizeToContents);
    ui->table->header()->setSectionResizeMode(COL_DESCRIPTION, QHeaderView::Stretch);
    ui->table->header()->setSectionResizeMode(COL_RESULT,      QHeaderView::ResizeToContents);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDateRange()
{
    QDate currentDate = QDate::currentDate();

    QDate dateFrom;

    if (currentDate.day() >= currentDate.dayOfWeek())
    {
        dateFrom = currentDate.addDays(1 - currentDate.dayOfWeek());
    }
    else
    {
        dateFrom = currentDate.addDays(1 - currentDate.day());
    }

    QDate dateTo = dateFrom.addDays(5 - dateFrom.dayOfWeek());

    if (dateTo.month() != dateFrom.month() || dateTo.year() != dateFrom.year())
    {
        dateTo = dateTo.addDays(-dateTo.day());
    }

    ui->dateFrom->setDate(dateFrom);
    ui->dateTo->setDate(dateTo);
}

void MainWindow::setItem(QTreeWidgetItem &item, const DialogTaskEdit &dialog)
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

QString MainWindow::toCsvValue(QString text)
{
    bool quoteString = text.contains(';') || text.contains('"');

    text.replace('"', "\"\"");

    if (quoteString)
    {
        text.append('"');
        text.prepend('"');
    }

    return text;

}

QString MainWindow::toCsvValue(const char *utf8ConstString)
{
    return toCsvValue(QString::fromUtf8(utf8ConstString));
}

void MainWindow::updateExportStatus()
{
    ui->labelExportStatus->setHidden(mDataExported);
}

void MainWindow::updateTotalHours()
{
    int hoursWork = 0;
    int hoursTotal = (ui->dateFrom->date().daysTo(ui->dateTo->date()) + 1) * 8;

    const int count = ui->table->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->table->topLevelItem(i);

        hoursWork += item->data(COL_HOURS_SPENT, Qt::UserRole).toInt();
    }

    ui->labelTotalHours->setText(QString::asprintf("%d / %d", hoursWork, hoursTotal));

    QPalette labelPalette = this->palette();

    if (hoursWork == hoursTotal)
    {
        labelPalette.setColor(QPalette::WindowText, QColor(56, 142, 60));
    }
    else
    {
        labelPalette.setColor(QPalette::WindowText, QColor(183, 28, 28));
    }

    ui->labelTotalHours->setPalette(labelPalette);

}

void MainWindow::dateRangeChanged()
{
    updateTotalHours();
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

    if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray taskByteArray = file.readAll();

    //// Load JSON =============================================================

    QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

    QJsonObject reportObject = taskDocument.object();

    mDataExported = reportObject["exported"].toBool();
    updateExportStatus();

    //// Check date range ------------------------------------------------------

    setupDateRange();

    QDate from = QDate::fromString(reportObject["from"].toString(), "dd.MM.yyyy");
    QDate to   = QDate::fromString(reportObject["to"].toString(), "dd.MM.yyyy");

    bool pastWeek = ((from.weekNumber() != ui->dateFrom->date().weekNumber()) || from.month() != ui->dateFrom->date().month());

    if (mDataExported && pastWeek)
    {
        //Not need to load already exported data from past week
        return;
    }
    else if (pastWeek && not mDataExported)
    {
        QMessageBox::warning(this, QString::fromUtf8("Экспорт"), QString::fromUtf8("Данные за прошлую неделю не были экспортированы"));
    }

    if (from.isValid())
    {
        ui->dateFrom->setDate(from);
    }

    if (to.isValid())
    {
        ui->dateTo->setDate(to);
    }

    //// Add tasks =============================================================

    foreach (const QJsonValue& value, reportObject["tasks"].toArray())
    {
        QJsonObject object = value.toObject();

        DialogTaskEdit dialog;
        dialog.setProjectTemplates(mProjectTemplates);

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

            //// Get result ----------------------------------------------------

            QByteArray resultStr = object["result"].toString().toUtf8();

            for (int i = 0; i < RESULT_COUNT; ++i)
            {
                if (QByteArray(gValuesResults[i].jsonValue) == resultStr)
                {
                    dialog.setTaskResult(gValuesResults[i].value);
                }
            }

            //// ---------------------------------------------------------------
        }

        //// Add item ----------------------------------------------------------

        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);

        ui->table->addTopLevelItem(item);
    }

    //// =======================================================================
}

void MainWindow::saveData()
{
    //// Generate JSON =========================================================

    QJsonObject reportObject;

    reportObject["from"]     = ui->dateFrom->date().toString("dd.MM.yyyy");
    reportObject["to"]       = ui->dateTo->date().toString("dd.MM.yyyy");
    reportObject["exported"] = mDataExported;

    QJsonArray taskArray;

    const int count = ui->table->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->table->topLevelItem(i);

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

            int resultId = item->data(COL_RESULT, Qt::UserRole).toInt();
            if (resultId >= ACTION_COUNT)
            {
                //TODO: error
                resultId = 0;
            }

            taskObject["project"]     = item->text(COL_PROJECT);
            taskObject["product"]     = item->text(COL_PRODUCT);
            taskObject["action"]      = QString::fromUtf8(gValuesActionTypes[actionId].jsonValue);
            taskObject["description"] = item->text(COL_DESCRIPTION);
            taskObject["result"]      = QString::fromUtf8(gValuesResults[resultId].jsonValue);
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
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать директорию для отчетов"));
            return;
        }
    }

    QString taskPath(taskDir.absoluteFilePath(".task.json"));
    QString taskPathBak(taskDir.absoluteFilePath(".task.json.bak"));

    QFile::remove(taskPathBak);
    QFile::rename(taskPath, taskPathBak);

    //// Save task backup file =================================================

    QFile taskFile(taskPath);

    if (not taskFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно записать файл отчета"));
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

        mDataExported = false;
        updateExportStatus();
        saveData();

        updateTotalHours();
    }
}

void MainWindow::on_actionTaskEdit_triggered()
{
    QList<QTreeWidgetItem*> items = ui->table->selectedItems();
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

        mDataExported = false;
        updateExportStatus();
        saveData();

        updateTotalHours();
    }
}

void MainWindow::on_actionTaskDelete_triggered()
{
    QList<QTreeWidgetItem*> items = ui->table->selectedItems();
    if (items.isEmpty())
    {
        return;
    }

    if (QMessageBox::question(this, "Delete", "Delete selected item?") != QMessageBox::Yes)
    {
        return;
    }

    mDataExported = false;
    updateExportStatus();

    foreach (QTreeWidgetItem* item, items)
    {
        delete item;
    }

    saveData();

    updateTotalHours();
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
    //// Get task file paths ===================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        taskDir.mkpath(mSettings.getWorkPath());

        if (not taskDir.cd(mSettings.getWorkPath()))
        {
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать директорию для отчетов"));
            return;
        }
    }

    QString monthDir = ui->dateFrom->date().toString("yyyy-MM");

    if (not taskDir.cd(monthDir))
    {
        taskDir.mkdir(monthDir);

        if (not taskDir.cd(monthDir))
        {
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать директорию для отчетов"));
            return;
        }
    }

    QString fileName = QString::fromUtf8("Отчет ") + getDateRangeString() + ".csv";

    QString reportPath(taskDir.absoluteFilePath(fileName));
    QString reportPathBak(taskDir.absoluteFilePath(fileName + ".bak"));

    QFile::remove(reportPathBak);
    QFile::rename(reportPath, reportPathBak);

    //// Open report file ======================================================

    QFile reportFile(reportPath);

    if (not reportFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно создать файл отчета"));
        return;
    }

    //// Generate CSV ==========================================================

    QTextStream reportString(&reportFile);
    reportString.setCodec("UTF-8");
    reportString.setGenerateByteOrderMark(true);

    const int count = ui->table->topLevelItemCount();

    reportString << toCsvValue("ФИО");
    reportString << ';';
    reportString << toCsvValue("Временной период");
    reportString << ';';

    for (int j = 0; j < COLUMN_COUNT; ++j)
    {   
        reportString << toCsvValue(ui->table->headerItem()->text(j));

        if (j < COLUMN_COUNT - 1)
        {
            reportString << ';';
        }
        else
        {
            reportString << '\n';
        }
    }

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->table->topLevelItem(i);

        reportString << toCsvValue(mSettings.getUserName());
        reportString << ';';
        reportString << toCsvValue(getDateRangeString());
        reportString << ';';

        for (int j = 0; j < COLUMN_COUNT; ++j)
        {
            reportString << toCsvValue(item->text(j));

            if (j < COLUMN_COUNT - 1)
            {
                reportString << ';';
            }
            else
            {
                reportString << '\n';
            }
        }
    }

    //// Close report file =====================================================

    reportFile.close();

    //// Remove backup file ====================================================

    QFile::remove(reportPathBak);

    //// Set data exported flag ================================================

    mDataExported = true;
    updateExportStatus();
    saveData();

    //// =======================================================================
}

void MainWindow::on_buttonSend_clicked()
{
    exportData();

    //// Get report file name ==================================================

    QDir taskDir;

    if (not taskDir.cd(mSettings.getWorkPath()))
    {
        QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно открыть директорию для отчетов"));
        return;
    }

    QString monthDir = ui->dateFrom->date().toString("yyyy-MM");

    if (not taskDir.cd(monthDir))
    {
        QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно открыть директорию для отчетов"));
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

    outlook.startDetached(program, arguments);

#endif
}

void MainWindow::on_buttonTemplates_clicked()
{
    DialogProjectTemplatesEdit dialog;
    dialog.setProjectTemplates(mProjectTemplates);

    if (dialog.exec() == QDialog::Accepted)
    {
        mProjectTemplates.clear();

        int id = 0;

        foreach (const QString& project, dialog.getProjects())
        {
            mProjectTemplates.addProject(project);

            foreach(const QString& product, dialog.getProducts(id))
            {
                mProjectTemplates.addProduct(project, product);
            }

            ++id;
        }

        if (not mProjectTemplates.save())
        {
            QMessageBox::critical(this, QString::fromUtf8("Сохранение"), QString::fromUtf8("Невозможно сохранить файл шаблонов"));
        }
    }
}

void MainWindow::on_buttonOpenReportDir_clicked()
{
    QUrl url = QUrl::fromLocalFile(mSettings.getWorkPath());
    QDesktopServices::openUrl(url);
}

void MainWindow::on_table_itemSelectionChanged()
{
    bool haveSelectedItems = (ui->table->selectedItems().count() > 0);

    ui->buttonEdit->setEnabled(haveSelectedItems);
    ui->buttonRemove->setEnabled(haveSelectedItems);
}
