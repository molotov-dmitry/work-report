#ifndef DIALOGSETTINGSEDIT_H
#define DIALOGSETTINGSEDIT_H

#include <QDialog>

namespace Ui {
class DialogSettingsEdit;
}

class DialogSettingsEdit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettingsEdit(QWidget *parent = 0);
    ~DialogSettingsEdit();

    QString getName() const;
    QString getMailTo() const;
    QString getReportDir() const;

    void setName(const QString& name);
    void setMailTo(const QString& mailTo);
    void setReportDir(const QString& reportDir);

#ifdef Q_OS_WIN
    QString getOutlookPath() const;
    void setOutlookPath(const QString& outlookPath);
#endif

private slots:
    void on_buttonSetReportDirectory_clicked();

#ifdef Q_OS_WIN
    void on_buttonSetOutlookPath_clicked();
#endif

private:
    Ui::DialogSettingsEdit *ui;
};

#endif // DIALOGSETTINGSEDIT_H
