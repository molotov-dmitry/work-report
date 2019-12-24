#ifndef DIALOGPROJECTTEMPLATESEDIT_H
#define DIALOGPROJECTTEMPLATESEDIT_H

#include <QDialog>

#include "projecttemplates.h"

class QTreeWidgetItem;

namespace Ui {
class DialogProjectTemplatesEdit;
}

class DialogProjectTemplatesEdit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProjectTemplatesEdit(QWidget *parent = 0);
    ~DialogProjectTemplatesEdit();

    void setProjectTemplates(const ProjectTemplates& templates);

    QStringList getProjects() const;
    QStringList getProducts(int index) const;

    void accept();

private slots:
    void on_buttonAdd_clicked();

    void on_buttonRemove_clicked();

    void on_buttonAddSub_clicked();

    void on_buttonUp_clicked();

    void on_buttonDown_clicked();

    void on_buttonSwitchMode_clicked();

private:
    Ui::DialogProjectTemplatesEdit *ui;

    void moveItem(QTreeWidgetItem* item, int move);
};

#endif // DIALOGPROJECTTEMPLATESEDIT_H
