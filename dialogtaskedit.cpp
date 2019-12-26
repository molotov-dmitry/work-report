#include "dialogtaskedit.h"
#include "ui_dialogtaskedit.h"

#include <QToolButton>
#include "values.h"

DialogTaskEdit::DialogTaskEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTaskEdit)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon::fromTheme("document-edit-symbolic", QIcon(":/icons/edit.svg")));

    const int HOUR_PRESETS[] = {4, 8, 16, 24, 32, 40};
    const int HOUR_PRESET_COUNT = sizeof(HOUR_PRESETS) / sizeof(HOUR_PRESETS[0]);

    for (int i = 0; i < HOUR_PRESET_COUNT; ++i)
    {
        QToolButton* toolButton = new QToolButton(this);

        toolButton->setText(QString::number(HOUR_PRESETS[i]));
        toolButton->setMinimumSize(32, 32);

        connect(toolButton, SIGNAL(clicked()), this, SLOT(setHoursPreset()));

        ui->layoutHourPresets->addWidget(toolButton);
    }

    //// Populate comboboxes ===================================================

    //// Task types ------------------------------------------------------------

    ui->boxType->clear();

    for (int i = 0; i < TASK_COUNT; ++i)
    {
        ui->boxType->addItem(QString::fromUtf8(gValuesTaskTypes[i].displayValue));
    }

    //// Action types ----------------------------------------------------------

    ui->boxAction->clear();

    for (int i = 0; i < ACTION_COUNT; ++i)
    {
        ui->boxAction->addItem(QString::fromUtf8(gValuesActionTypes[i].displayValue));
    }

    //// Result types ----------------------------------------------------------

    ui->boxResult->clear();

    for (int i = 0; i < RESULT_COUNT; ++i)
    {
        ui->boxResult->addItem(QString::fromUtf8(gValuesResults[i].displayValue));
    }

    //// =======================================================================
}

DialogTaskEdit::~DialogTaskEdit()
{
    delete ui;
}

int DialogTaskEdit::getTaskType() const
{
    return ui->boxType->currentIndex();
}

QString DialogTaskEdit::getTaskTypeString() const
{
    return ui->boxType->itemText(getTaskType());
}

int DialogTaskEdit::getTaskHoursSpent() const
{
    return ui->editHoursSpent->value();
}

QString DialogTaskEdit::getTaskProject() const
{
    return ui->editProject->currentText();
}

QString DialogTaskEdit::getTaskProduct() const
{
    return ui->editProduct->currentText();
}

int DialogTaskEdit::getTaskActionType() const
{
    return ui->boxAction->currentIndex();
}

QString DialogTaskEdit::getTaskActionTypeString() const
{
    return ui->boxAction->itemText(getTaskActionType());
}

QString DialogTaskEdit::getTaskDescription() const
{
    return ui->editDescription->text().replace("\n", "\r");
}

int DialogTaskEdit::getTaskResult() const
{
    return ui->boxResult->currentIndex();
}

QString DialogTaskEdit::getTaskResultString() const
{
    return ui->boxResult->itemText(getTaskResult());
}

void DialogTaskEdit::setPlanMode(bool planMode)
{
    ui->boxResult->setDisabled(planMode);
}

void DialogTaskEdit::setTaskType(int taskType)
{
    ui->boxType->setCurrentIndex(taskType);
}

void DialogTaskEdit::setTaskHoursSpent(int hours)
{
    ui->editHoursSpent->setValue(hours);
}

void DialogTaskEdit::setTaskProject(const QString &project)
{
    ui->editProject->setCurrentText(project);
}

void DialogTaskEdit::setTaskProduct(const QString &product)
{
    ui->editProduct->setCurrentText(product);
}

void DialogTaskEdit::setTaskActionType(int actionType)
{
    ui->boxAction->setCurrentIndex(actionType);
}

void DialogTaskEdit::setTaskDescription(const QString &description)
{
    ui->editDescription->setText(description);
}

void DialogTaskEdit::setTaskResult(int result)
{
    ui->boxResult->setCurrentIndex(result);
}

void DialogTaskEdit::setProjectTemplates(const ProjectTemplates &projects)
{
    mProjects = projects.getData();

    ui->editProject->clear();

    foreach(QString project, projects.getProjects())
    {
        ui->editProject->addItem(project);
    }
}

void DialogTaskEdit::setHoursPreset()
{
    QToolButton* button = static_cast<QToolButton*>(sender());

    ui->editHoursSpent->setValue(button->text().toInt());
}

void DialogTaskEdit::on_editProject_currentTextChanged(const QString &arg1)
{
    QString text = ui->editProduct->currentText();

    ui->editProduct->clear();

    foreach(QString project, mProjects.value(arg1))
    {
        ui->editProduct->addItem(project);
    }

    if (not text.isEmpty())
    {
        ui->editProduct->setCurrentText(text);
    }
}
