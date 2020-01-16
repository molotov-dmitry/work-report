#ifndef MONTHDAYSSELECTWIDGET_H
#define MONTHDAYSSELECTWIDGET_H

#include <QWidget>
#include <QList>

namespace Ui {
class MonthDaysSelectWidget;
}

class QToolButton;
class QDate;

class MonthDaysSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MonthDaysSelectWidget(QWidget *parent = nullptr);
    ~MonthDaysSelectWidget();

    int workDaysCount() const;
    QList<int> workDays() const;

    void setDate(const QDate& date);

    void setWorkDays(QList<int> days);

signals:

    void daysUpdated();

private:
    Ui::MonthDaysSelectWidget *ui;

    QList<QToolButton*> buttons;

    void clear();
    void reload(int month, int year);

private slots:

    void checkDay(bool toggled);
};

#endif // MONTHDAYSSELECTWIDGET_H
