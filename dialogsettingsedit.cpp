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
