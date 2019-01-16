#include "dialogtaskedit.h"
#include "ui_dialogtaskedit.h"

DialogTaskEdit::DialogTaskEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTaskEdit)
{
    ui->setupUi(this);
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
    return ui->editDescription->text();
}

int DialogTaskEdit::getTaskResult() const
{
    return ui->boxResult->currentIndex();
}

QString DialogTaskEdit::getTaskResultString() const
{
    return ui->boxResult->itemText(getTaskResult());
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
