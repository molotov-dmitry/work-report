#include "dialogprojecttemplatesedit.h"
#include "ui_dialogprojecttemplatesedit.h"

DialogProjectTemplatesEdit::DialogProjectTemplatesEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProjectTemplatesEdit)
{
    ui->setupUi(this);
}

DialogProjectTemplatesEdit::~DialogProjectTemplatesEdit()
{
    delete ui;
}
