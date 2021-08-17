#ifndef DIALOGTASKEDIT_H
#define DIALOGTASKEDIT_H

#include <QDialog>

#include "projecttemplates.h"
#include "plannedtasks.h"

namespace Ui {
class DialogTaskEdit;
}

class DialogTaskEdit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTaskEdit(QWidget *parent = 0);
    ~DialogTaskEdit();

    int     getTaskType() const;
    QString getTaskTypeString() const;
    int     getTaskHoursSpent() const;

    QString getTaskProject() const;
    QString getTaskProduct() const;
    int     getTaskActionType() const;
    QString getTaskActionTypeString() const;
    QString getTaskDescription() const;
    QString getTaskPlan() const;

    int     getTaskResult() const;
    QString getTaskResultString() const;


    void setPlanMode(bool planMode);

    void setTaskType(int taskType);
    void setTaskHoursSpent(int hours);

    void setTaskProject(const QString& project);
    void setTaskProduct(const QString& product);
    void setTaskActionType(int actionType);
    void setTaskDescription(const QString& description);
    void setTaskPlan(const QString& plan);
    void setTaskResult(int result);

    void setProjectTemplates(const ProjectTemplates& projects);
    void setPlannedTasks(const PlannedTasks& tasks);

protected slots:

    void accept() override;

private slots:

    void setHoursPreset();

    void updatePlannedTask();

    void on_editProject_currentTextChanged(const QString &arg1);
    void on_editProduct_currentTextChanged(const QString &arg1);
    void on_boxAction_currentIndexChanged(int index);

private:
    Ui::DialogTaskEdit *ui;

    QMap<QString, QStringList> mProjects;
    QMap< QPair<QString, QString>, QList<PlannedTask> > mPlannedTasks;
};

#endif // DIALOGTASKEDIT_H
