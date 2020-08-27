#ifndef PLANNEDTASKS_H
#define PLANNEDTASKS_H

#include <QMap>
#include <QPair>
#include <QString>
#include <QList>

class QDate;
class Settings;

struct PlannedTask
{
    QString description;
    int     action;
    int     hours;
};

class PlannedTasks
{
public:
    explicit PlannedTasks(const Settings& settings);
    explicit PlannedTasks(const Settings& settings, const QDate& date);
    explicit PlannedTasks(const PlannedTasks& other);

    void load(const QDate& date);

    QMap< QPair<QString, QString>, QList<PlannedTask> > getPlannedTasks() const;

private:
    QMap< QPair<QString, QString>, QList<PlannedTask> > mPlannedTasks;
    const Settings& mSettings;
};

#endif // PLANNEDTASKS_H
