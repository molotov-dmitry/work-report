#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogtaskedit.h"

#include <QDate>
#include <QMessageBox>

#include <QDebug>

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

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

void MainWindow::loadData()
{
    QFile file(".task.json");

    if (not file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QByteArray taskByteArray = file.readAll();

    QJsonDocument taskDocument = QJsonDocument::fromJson(taskByteArray);

    foreach (const QJsonValue& value, taskDocument.array())
    {
        QJsonObject object = value.toObject();

        DialogTaskEdit dialog;

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
}

void MainWindow::saveData()
{
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

    QFile file(".task.json");

    file.remove(".task.json.old");

    file.rename(".task.json", ".task.json.old");

    file.open(QIODevice::WriteOnly);

    file.write(taskDocument.toJson());

    file.close();

}

void MainWindow::exportData()
{
    QString taskString;

    const int count = ui->table->topLevelItemCount();

    taskString.append("Сотрудник");
    taskString.append("\t");
    taskString.append("Диапазон");
    taskString.append("\t");

    for (int j = 0; j < COLUMN_COUNT; ++j)
    {
        taskString.append(ui->table->headerItem()->text(j));

        if (j < COLUMN_COUNT - 1)
        {
            taskString.append("\t");
        }
        else
        {
            taskString.append("\n");
        }
    }

    for (int i = 0; i < count; ++i)
    {
        const QTreeWidgetItem* item = ui->table->topLevelItem(i);

        taskString.append("Имя");
        taskString.append("\t");
        taskString.append(ui->dateFrom->date().toString("dd.MM.yyyy") + " - " + ui->dateTo->date().toString("dd.MM.yyyy"));
        taskString.append("\t");

        for (int j = 0; j < COLUMN_COUNT; ++j)
        {
            taskString.append(item->text(j));

            if (j < COLUMN_COUNT - 1)
            {
                taskString.append("\t");
            }
            else
            {
                taskString.append("\n");
            }
        }
    }

    QFile file("task.csv");

    file.open(QIODevice::WriteOnly);

    file.write(taskString.toUtf8());

    file.close();
}

void MainWindow::on_actionTaskNew_triggered()
{
    DialogTaskEdit dialog;

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
