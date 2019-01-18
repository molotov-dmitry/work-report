#include "dialogsettingsedit.h"
#include "ui_dialogsettingsedit.h"

#include <QFileDialog>

DialogSettingsEdit::DialogSettingsEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettingsEdit)
{
    ui->setupUi(this);
}

DialogSettingsEdit::~DialogSettingsEdit()
{
    delete ui;
}

QString DialogSettingsEdit::getName() const
{
    return ui->editName->text();
}

QString DialogSettingsEdit::getMailTo() const
{
    return ui->editMailTo->text();
}

QString DialogSettingsEdit::getReportDir() const
{
    return ui->editReportDir->text();
}

void DialogSettingsEdit::setName(const QString &name)
{
    ui->editName->setText(name);
}

void DialogSettingsEdit::setMailTo(const QString &mailTo)
{
    ui->editMailTo->setText(mailTo);
}

void DialogSettingsEdit::setReportDir(const QString &reportDir)
{
    ui->editReportDir->setText(reportDir);
}

void DialogSettingsEdit::on_buttonSetReportDirectory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Directory"),
                                                    ui->editReportDir->text(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (not dir.isEmpty())
    {
        ui->editReportDir->setText(dir);
    }
}
