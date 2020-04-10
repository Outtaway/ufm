#include "QuickPanel.h"

#include <QTreeWidget>

QuickPanel::QuickPanel(QTreeWidget* tree_view) :
    quick_panel_widget_(tree_view)
{
    setup();
}

void QuickPanel::setup()
{
    QTreeWidgetItem* quick_access = addSection_(quick_panel_widget_);
    sections_[QUICK_ACCESS_SECTION] = std::make_unique<QuickAccessSection>(quick_access, QUICK_ACCESS_SECTION);
    sections_[QUICK_ACCESS_SECTION]->setup();

    QTreeWidgetItem* recent = addSection_(quick_panel_widget_);
    sections_[RECENT_SECTION] = std::make_unique<RecentSection>(recent, RECENT_SECTION);
    sections_[RECENT_SECTION]->setup();
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

QString QuickPanel::getPathByName(QString section, QString name)
{
    return sections_[section]->getPathByName(name);
}

bool QuickPanel::sectionExists(QString section)
{
    return sections_.find(section) != sections_.end();
}
