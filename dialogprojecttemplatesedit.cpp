#include "dialogprojecttemplatesedit.h"
#include "ui_dialogprojecttemplatesedit.h"

#include <QTreeWidgetItem>

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

void DialogProjectTemplatesEdit::setProjectTemplates(const ProjectTemplates &templates)
{
    foreach(const QString& project, templates.getProjects())
    {
        QTreeWidgetItem* projectItem = new QTreeWidgetItem();
        projectItem->setText(0, project);
        projectItem->setFlags(projectItem->flags() | Qt::ItemIsEditable);

        ui->table->addTopLevelItem(projectItem);
        ui->table->expandItem(projectItem);

        foreach(const QString& product, templates.getData().value(project))
        {
            QTreeWidgetItem* productItem = new QTreeWidgetItem();

            productItem->setFlags(productItem->flags() | Qt::ItemIsEditable);
            productItem->setText(0, product);
            projectItem->addChild(productItem);
        }
    }
}

QStringList DialogProjectTemplatesEdit::getProjects() const
{
    QStringList result;

    int count = ui->table->topLevelItemCount();

    for (int i = 0; i < count; ++i)
    {
        result.append(ui->table->topLevelItem(i)->text(0));
    }

    return result;
}

QStringList DialogProjectTemplatesEdit::getProducts(int index) const
{
    QStringList result;

    QTreeWidgetItem* root = ui->table->topLevelItem(index);

    if (root)
    {
        int count = root->childCount();

        for (int i = 0; i < count; ++i)
        {
            result.append(root->child(i)->text(0));
        }
    }

    return result;
}

void DialogProjectTemplatesEdit::on_buttonAdd_clicked()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (not ui->table->selectedItems().empty())
    {
        QTreeWidgetItem* projectItem = ui->table->selectedItems().first();

        if (projectItem->parent() != Q_NULLPTR)
        {
            projectItem = projectItem->parent();
        }

        if (projectItem == Q_NULLPTR)
        {
            ui->table->addTopLevelItem(item);
        }
        else
        {
            ui->table->insertTopLevelItem(ui->table->indexOfTopLevelItem(projectItem) + 1, item);
        }
    }
    else
    {
        ui->table->addTopLevelItem(item);
    }

    foreach (QTreeWidgetItem* selectedItem, ui->table->selectedItems())
    {
        selectedItem->setSelected(false);
    }

    item->setSelected(true);
    ui->table->editItem(item, 0);
}

void DialogProjectTemplatesEdit::on_buttonRemove_clicked()
{
    foreach (QTreeWidgetItem* item, ui->table->selectedItems())
    {
        delete item;
    }
}

void DialogProjectTemplatesEdit::on_buttonAddSub_clicked()
{
    if (not ui->table->selectedItems().empty())
    {
        QTreeWidgetItem* insertAfter = Q_NULLPTR;
        QTreeWidgetItem* projectItem = ui->table->selectedItems().first();

        if (projectItem->parent() != Q_NULLPTR)
        {
            insertAfter = projectItem;
            projectItem = projectItem->parent();
        }

        QTreeWidgetItem* item = new QTreeWidgetItem();

        item->setFlags(item->flags() | Qt::ItemIsEditable);

        if (insertAfter == Q_NULLPTR)
        {
            projectItem->addChild(item);
        }
        else
        {
            projectItem->insertChild(projectItem->indexOfChild(insertAfter) + 1, item);
        }

        foreach (QTreeWidgetItem* selectedItem, ui->table->selectedItems())
        {
            selectedItem->setSelected(false);
        }

        ui->table->expandItem(projectItem);
        item->setSelected(true);
        ui->table->editItem(item, 0);
    }
}

void DialogProjectTemplatesEdit::on_buttonUp_clicked()
{
    if (not ui->table->selectedItems().empty())
    {
        moveItem(ui->table->selectedItems().first(), -1);
    }
}

void DialogProjectTemplatesEdit::on_buttonDown_clicked()
{
    if (not ui->table->selectedItems().empty())
    {
        moveItem(ui->table->selectedItems().first(), 1);
    }
}

void DialogProjectTemplatesEdit::moveItem(QTreeWidgetItem *item, int move)
{
    if (item == Q_NULLPTR)
    {
        return;
    }

    if (item->parent())
    {
        QTreeWidgetItem* parentItem = item->parent();

        int index = parentItem->indexOfChild(item);

        if (index + move < 0 || index + move >= parentItem->childCount())
        {
            return;
        }

        QTreeWidgetItem* child = parentItem->takeChild(index);

        parentItem->insertChild(index + move, child);
    }
    else
    {
        int index = ui->table->indexOfTopLevelItem(item);

        if (index + move < 0 || index + move >= ui->table->topLevelItemCount())
        {
            return;
        }

        QTreeWidgetItem* child = ui->table->takeTopLevelItem(index);

        ui->table->insertTopLevelItem(index + move, child);
    }

    foreach (QTreeWidgetItem* selectedItem, ui->table->selectedItems())
    {
        selectedItem->setSelected(false);
    }

    item->setSelected(true);
}
