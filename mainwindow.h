#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMap>
#include <QStringList>

#include "settings.h"
#include "projecttemplates.h"

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

    COLUMN_COUNT
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

    void loadData();
    void saveData();

    void on_buttonSettings_clicked();

    void exportData();

    void on_buttonSend_clicked();

    void on_buttonTemplates_clicked();

private:
    Ui::MainWindow *ui;

    Settings mSettings;

    ProjectTemplates mProjectTemplates;

    void setupDateRange();

    void setItem(QTreeWidgetItem& item, const DialogTaskEdit& dialog);

    QString getDateRangeString() const;


};

#endif // MAINWINDOW_H
