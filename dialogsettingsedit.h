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

private slots:
    void on_buttonSetReportDirectory_clicked();

private:
    Ui::DialogSettingsEdit *ui;
};

#endif // DIALOGSETTINGSEDIT_H
