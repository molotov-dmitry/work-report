#include "dialogprojecttemplatesedit.h"
#include "ui_dialogprojecttemplatesedit.h"

#include <QTreeWidgetItem>
#include <QFontDatabase>
#include <QUrl>
#include <QDesktopServices>

DialogProjectTemplatesEdit::DialogProjectTemplatesEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProjectTemplatesEdit)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon::fromTheme("folder-templates-symbolic", QIcon(":/icons/template.svg")));

    ui->buttonAdd->setIcon(QIcon::fromTheme("folder-add", QIcon(":/icons/project-add.svg")));
    ui->buttonAddSub->setIcon(QIcon::fromTheme("document-add", QIcon(":/icons/product-add.svg")));
    ui->buttonRemove->setIcon(QIcon::fromTheme("edit-delete-symbolic", QIcon(":/icons/delete.svg")));

    ui->buttonUp->setIcon(QIcon::fromTheme("up", QIcon(":/icons/move-up.svg")));
    ui->buttonDown->setIcon(QIcon::fromTheme("down", QIcon(":/icons/move-down.svg")));

    ui->buttonSwitchMode->setIcon(QIcon::fromTheme("text", QIcon(":/icons/text.svg")));

    ui->buttonGitLab->setIcon(QIcon::fromTheme("gitlab", QIcon(":/icons/gitlab.svg")));

    ui->editTemplates->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
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

void DialogProjectTemplatesEdit::accept()
{
    if (ui->stackedWidget->currentIndex() == 1)
    {
        on_buttonSwitchMode_clicked();
    }

    QDialog::accept();
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

    ui->table->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
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

        ui->table->expandItem(projectItem);

        ui->table->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
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

    ui->table->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
    ui->table->expandItem(item);
}

void DialogProjectTemplatesEdit::on_buttonSwitchMode_clicked()
{
    if (ui->stackedWidget->currentIndex() == 0)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->buttonSwitchMode->setIcon(QIcon::fromTheme("view-list-tree", QIcon(":/icons/tree.svg")));
        ui->buttonSwitchMode->setText(QString::fromUtf8("Древовидный режим"));

        ui->editTemplates->clear();

        for (int i = 0; i < ui->table->topLevelItemCount(); ++i)
        {
            QTreeWidgetItem* projectItem = ui->table->topLevelItem(i);

            ui->editTemplates->appendPlainText(projectItem->text(0));

            for (int j = 0; j < projectItem->childCount(); ++j)
            {
                QTreeWidgetItem* productItem = projectItem->child(j);

                ui->editTemplates->appendPlainText(QString(" ") + productItem->text(0));
            }
        }
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(0);
        ui->buttonSwitchMode->setIcon(QIcon::fromTheme("text", QIcon(":/icons/text.svg")));
        ui->buttonSwitchMode->setText(QString::fromUtf8("Текстовый режим"));

        ui->table->clear();

        QStringList lines = ui->editTemplates->toPlainText().split('\n');

        QTreeWidgetItem* currentProject = Q_NULLPTR;

        foreach (const QString& line, lines)
        {
            if (line.isEmpty())
            {
                continue;
            }

            if (line.startsWith(' '))
            {
                if (currentProject == Q_NULLPTR)
                {
                    currentProject = new QTreeWidgetItem();
                    ui->table->addTopLevelItem(currentProject);
                }

                QTreeWidgetItem* currentProduct = new QTreeWidgetItem();
                currentProduct->setText(0, line.mid(1));

                currentProject->addChild(currentProduct);
            }
            else
            {
                currentProject = new QTreeWidgetItem();
                currentProject->setText(0, line);
                ui->table->addTopLevelItem(currentProject);
            }

            ui->table->expandItem(currentProject);
        }
    }
}

void DialogProjectTemplatesEdit::on_buttonGitLab_clicked()
{
    if (ui->stackedWidget->currentIndex() == 0)
    {
        on_buttonSwitchMode_clicked();
    }

    QUrl url("https://git.rczifort.local/documents/project-templates/blob/master/README.md");

    QDesktopServices::openUrl(url);
}
