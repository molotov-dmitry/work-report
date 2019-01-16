#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QTreeWidgetItem;
class DialogTaskEdit;

enum Column
{
    COL_TYPE,
    COL_HOURS_SPENT,
    COL_PROJECT,
    COL_PRODUCT,
    COL_ACTION,
    COL_DESCRIPTION,
    COL_RESULT,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionTaskNew_triggered();

    void on_actionTaskEdit_triggered();

    void on_actionTaskDelete_triggered();

private:
    Ui::MainWindow *ui;

    void setupDateRange();

    void setItem(QTreeWidgetItem& item, const DialogTaskEdit& dialog);
};

#endif // MAINWINDOW_H
