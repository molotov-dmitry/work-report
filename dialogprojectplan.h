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
    void on_buttonAdd_clicked();
    void on_buttonEdit_clicked();
    void on_buttonRemove_clicked();

    void updatePlanDate(const QDate &date);

    void updatePlanHours();

    void changeDate();

    void loadPlan();
    void loadMonthReport();

    void savePlan();
    void exportPlan();

    void on_buttonMoveBack_clicked();
    void on_buttonMoveForward_clicked();

    void moveInTime(int months);

    void on_buttonGoToday_clicked();

private:
    Ui::DialogProjectPlan *ui;

    const ProjectTemplates& mProjectTemplates;
    const Settings&         mSettings;

    void setItem(QTreeWidgetItem& item, const DialogTaskEdit& dialog);
};

#endif // DIALOGPROJECTPLAN_H
