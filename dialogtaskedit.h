#ifndef DIALOGTASKEDIT_H
#define DIALOGTASKEDIT_H

#include <QDialog>

#include "projecttemplates.h"

enum TaskType
{
    TASK_ACTION,
    TASK_VACATION,
    TASK_ILLNESS
};

enum TaskActionType
{
    ACTION_CREATE,
    ACTION_MODIFY,
    ACTION_TEST,
    ACTION_BUSINESS_TRIP,
    ACTION_DOCS
};

enum TaskResult
{
    RESULT_DONE,
    RESULT_DONE_PART,
    RESULT_STUCK
};

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

    int     getTaskResult() const;
    QString getTaskResultString() const;


    void setTaskType(int taskType);
    void setTaskHoursSpent(int hours);

    void setTaskProject(const QString& project);
    void setTaskProduct(const QString& product);
    void setTaskActionType(int actionType);
    void setTaskDescription(const QString& description);
    void setTaskResult(int result);

    void setProjectTemplates(const ProjectTemplates& projects);

private slots:

    void setHoursPreset();

    void on_editProject_currentTextChanged(const QString &arg1);

private:
    Ui::DialogTaskEdit *ui;

    QMap<QString, QStringList> mProjects;
};

#endif // DIALOGTASKEDIT_H
