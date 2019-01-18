#ifndef DIALOGPROJECTTEMPLATESEDIT_H
#define DIALOGPROJECTTEMPLATESEDIT_H

#include <QDialog>

namespace Ui {
class DialogProjectTemplatesEdit;
}

class DialogProjectTemplatesEdit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProjectTemplatesEdit(QWidget *parent = 0);
    ~DialogProjectTemplatesEdit();

private:
    Ui::DialogProjectTemplatesEdit *ui;
};

#endif // DIALOGPROJECTTEMPLATESEDIT_H
