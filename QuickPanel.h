#pragma once

#include <map>
#include <memory>

#include "SectionBase.h"
#include "QuickAccessSection.h"
#include "RecentSection.h"

class QTreeWidget;

class QuickPanel
{
public:

    QuickPanel(QTreeWidget* tree_view);

private:

    void setup();

public:

    void addItemToSection(QString section, QString name, QString path);

    QString getPathByName(QString section, QString name);

    bool sectionExists(QString section);

    static constexpr const char* const QUICK_ACCESS_SECTION = "Quick access";

    static constexpr const char* const RECENT_SECTION = "Recent";

private:

    QTreeWidgetItem* addSection_(QTreeWidget* parent);

    QTreeWidget* quick_panel_widget_;

    std::map<QString, std::unique_ptr<SectionBase>> sections_;

};
