#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogtaskedit.h"

#include <QDate>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(on_actionTaskNew_triggered()));
    connect(ui->buttonEdit, SIGNAL(clicked()), this, SLOT(on_actionTaskEdit_triggered()));
    connect(ui->buttonRemove, SIGNAL(clicked()), this, SLOT(on_actionTaskDelete_triggered()));
    connect(ui->table, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(on_actionTaskEdit_triggered()));

    setupDateRange();
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
    item.setText(COL_HOURS_SPENT, QString::asprintf("%d ч.", dialog.getTaskHoursSpent()));

    if (dialog.getTaskType() == TASK_ACTION)
    {
        item.setText(COL_PROJECT, dialog.getTaskProject());

        item.setText(COL_PRODUCT, dialog.getTaskProject());

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

void MainWindow::on_actionTaskNew_triggered()
{
    DialogTaskEdit dialog;

    if (dialog.exec() == QDialog::Accepted)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem;

        setItem(*item, dialog);

        ui->table->addTopLevelItem(item);
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
}
