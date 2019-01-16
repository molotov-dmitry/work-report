#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
