#ifndef DIALOGPROJECTPLAN_H
#define DIALOGPROJECTPLAN_H

#include <QDialog>

class ProjectTemplates;
class QTreeWidgetItem;
class DialogTaskEdit;
class Settings;

namespace Ui {
class DialogProjectPlan;
}

class DialogProjectPlan : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProjectPlan(const ProjectTemplates& projectTemplates,
                               const Settings& settings,
                               QWidget *parent = nullptr);
    ~DialogProjectPlan();

private slots:
    void on_buttonAddPlan_clicked();
    void on_buttonEditPlan_clicked();
    void on_buttonRemovePlan_clicked();

    void updatePlanDate(const QDate &date);

    void updatePlanHours();

    void changeDate();

    void loadPlan();
    void loadMonthReport();

    void loadMonthTasks();
    void updateMonthReportHours();

    void savePlan();
    void saveMonthReport();

    void exportPlan();

    void on_buttonMoveBack_clicked();
    void on_buttonMoveForward_clicked();

    void moveInTime(int months);

    void on_buttonGoToday_clicked();

private:
    Ui::DialogProjectPlan *ui;

    const ProjectTemplates& mProjectTemplates;
    const Settings&         mSettings;

    void setItem(QTreeWidgetItem& item, const DialogTaskEdit& dialog, bool isReport = false);
};

#endif // DIALOGPROJECTPLAN_H
