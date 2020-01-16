#include "monthdaysselectwidget.h"
#include "ui_monthdaysselectwidget.h"

#include <QDate>
#include <QLocale>
#include <QLabel>
#include <QToolButton>

MonthDaysSelectWidget::MonthDaysSelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthDaysSelectWidget)
{
    ui->setupUi(this);

    QLocale locale;

    for (int i = 1; i <= 7; ++i)
    {
        QLabel* label = new QLabel(this);
        label->setText(locale.standaloneDayName(i, QLocale::ShortFormat));
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        if (i > locale.weekdays().count())
        {
            QPalette palette = this->palette();
            palette.setColor(QPalette::WindowText, Qt::darkRed);
            label->setPalette(palette);
        }

        ui->layout->addWidget(label, 0, i - 1, 1, 1);
    }

    QDate currentDate = QDate::currentDate();

    reload(currentDate.month(), currentDate.year());
}

MonthDaysSelectWidget::~MonthDaysSelectWidget()
{
    delete ui;
}

int MonthDaysSelectWidget::workDaysCount() const
{
    int result = 0;

    foreach (QToolButton* button, buttons)
    {
        if (button->isChecked())
        {
            ++result;
        }
    }

    return result;
}

QList<int> MonthDaysSelectWidget::workDays() const
{
    QList<int> result;

    for (int i = 0; i < buttons.count(); ++i)
    {
        if (buttons.at(i)->isChecked())
        {
            result.append(i + 1);
        }
    }

    return result;
}

void MonthDaysSelectWidget::setDate(const QDate& date)
{
    reload(date.month(), date.year());
}

void MonthDaysSelectWidget::setWorkDays(QList<int> days)
{
    this->blockSignals(true);

    for (int i = 0; i < buttons.count(); ++i)
    {
        buttons.at(i)->setChecked(days.contains(i + 1));
    }

    this->blockSignals(false);
}

void MonthDaysSelectWidget::clear()
{
    foreach (QToolButton* button, buttons)
    {
        ui->layout->removeWidget(button);
        button->hide();
        button->setParent(nullptr);
        button->deleteLater();
    }

    buttons.clear();
}

void MonthDaysSelectWidget::reload(int month, int year)
{
    this->blockSignals(true);

    clear();

    QLocale locale;

    QDate date(year, month, 1);

    int offset = date.dayOfWeek() - 1;

    for (int i = 1; i <= date.daysInMonth(); ++i)
    {
        int x = offset % 7;
        int y = (offset / 7) + 1;

        QToolButton* button = new QToolButton(this);
        connect(button, SIGNAL(toggled(bool)), this, SLOT(checkDay(bool)));
        button->setText(QString::number(i));
        button->setCheckable(true);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        if (x + 1 > locale.weekdays().count())
        {
            QPalette palette = this->palette();
            palette.setColor(QPalette::ButtonText, Qt::darkRed);
            button->setPalette(palette);
        }
        else
        {
            button->setChecked(true);
        }

        ui->layout->addWidget(button, y, x, 1, 1);

        buttons.append(button);

        ++offset;
    }

    this->blockSignals(false);

}

void MonthDaysSelectWidget::checkDay(bool toggled)
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());

    if (button != nullptr)
    {
        QFont font = button->font();

        font.setBold(toggled);

        button->setFont(font);

        emit daysUpdated();
    }
}
