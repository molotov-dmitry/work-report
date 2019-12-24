#include "dialogsettingsedit.h"
#include "ui_dialogsettingsedit.h"

#include <QFileDialog>
#include <QStandardPaths>

DialogSettingsEdit::DialogSettingsEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettingsEdit)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon::fromTheme("preferences-system-symbolic", QIcon(":/icons/settings.svg")));

#ifndef Q_OS_WIN
   ui->widgetOutlookPath->hide();
#endif
}

DialogSettingsEdit::~DialogSettingsEdit()
{
    delete ui;
}

QString DialogSettingsEdit::getFirstName() const
{
    return ui->editFirstName->text();
}

QString DialogSettingsEdit::getSurname() const
{
    return ui->editSurname->text();
}

QString DialogSettingsEdit::getLastName() const
{
    return ui->editLastName->text();
}

QString DialogSettingsEdit::getDepartment() const
{
    return ui->editDepartment->text();
}

QString DialogSettingsEdit::getMailTo() const
{
    return ui->editMailTo->text();
}

QString DialogSettingsEdit::getReportDir() const
{
    return ui->editReportDir->text();
}

void DialogSettingsEdit::setFirstName(const QString& firstName)
{
    ui->editFirstName->setText(firstName);
}

void DialogSettingsEdit::setSurname(const QString& surname)
{
    ui->editSurname->setText(surname);
}

void DialogSettingsEdit::setLastName(const QString& lastName)
{
    ui->editLastName->setText(lastName);
}

void DialogSettingsEdit::setDepartment(const QString& department)
{
    ui->editDepartment->setText(department);
}

void DialogSettingsEdit::setMailTo(const QString &mailTo)
{
    ui->editMailTo->setText(mailTo);
}

void DialogSettingsEdit::setReportDir(const QString &reportDir)
{
    ui->editReportDir->setText(reportDir);
}

#ifdef Q_OS_WIN
QString DialogSettingsEdit::getOutlookPath() const
{
    return ui->editOutlookPath->text();
}

void DialogSettingsEdit::setOutlookPath(const QString &outlookPath)
{
    ui->editOutlookPath->setText(outlookPath);
}
#endif

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

#ifdef Q_OS_WIN
void DialogSettingsEdit::on_buttonSetOutlookPath_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Open File"), getenv("PROGRAMFILES"), "outlook.exe");

    if (not dir.isEmpty())
    {
        ui->editOutlookPath->setText(dir);
    }
}
#endif
