#include "QuickPanel.h"

#include <QTreeWidget>
#include <QDebug>

QuickPanel::QuickPanel(QTreeWidget* tree_view) :
    quick_panel_widget_(tree_view)
{
    setup();
}

void QuickPanel::setup()
{
    QTreeWidgetItem* quick_access = addSection_(quick_panel_widget_);
    sections_[QUICK_ACCESS_SECTION] = std::make_unique<QuickAccessSection>(quick_access, QUICK_ACCESS_SECTION);

    QTreeWidgetItem* recent = addSection_(quick_panel_widget_);
    sections_[RECENT_SECTION] = std::make_unique<RecentSection>(recent, RECENT_SECTION);
}

QTreeWidgetItem* QuickPanel::addSection_(QTreeWidget* parent)
{
    QTreeWidgetItem* new_category = new QTreeWidgetItem(parent);
    quick_panel_widget_->addTopLevelItem(new_category);

    return new_category;
}

void QuickPanel::addItemToSection(QString section, QString name, QString path)
{
    sections_[section]->addItem(name, path);
    sections_[section]->updateUi();
}

void QuickPanel::deleteItemFromSection(QString section, QString name)
{
    if (!sectionExists(section))
        return;

    sections_[section]->deleteItem(name);
    sections_[section]->updateUi();
}

void QuickPanel::clearSelection()
{
    quick_panel_widget_->clearSelection();
}

QString QuickPanel::getSelected()
{
    QString item_name = quick_panel_widget_->currentItem()->text(0);

    return item_name;
}

QString QuickPanel::getPathByName(QString section, QString name)
{
    return sections_[section]->getPathByName(name);
}

bool QuickPanel::sectionExists(QString section)
{
    return sections_.find(section) != sections_.end();
}

bool QuickPanel::isSection(QTreeWidgetItem* item_to_check, QString section)
{
    if (item_to_check == nullptr)
        return false;
    return item_to_check->text(0) == section;
}
