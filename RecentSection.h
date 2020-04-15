#pragma once

#include <map>
#include <list>
#include <memory>

#include <QString>

#include "SectionBase.h"

class QSqlDatabase;

class RecentSection : public SectionBase
{
public:

    RecentSection(QTreeWidgetItem* section, QString name);

    ~RecentSection();

    void setup() override;

    void addItem(QString name, QString path) override;

    void updateUi() override;

    QString getPathByName(QString name) override;

private:

    void moveRecentToTop_(QString name);

    bool recentExists_(QString name);

    bool openDatabase_();

    bool importRecentsFromDatabase_();

    bool exportRecentsToDatabase_();

    enum RECENT_TABLE_COLUMNS
    {
        NAME = 0,
        PATH
    };
    const char* const RECENT_DB_NAME = "recent.db";
    const char* const RECENT_TABLE_NAME = "recent";
    std::unique_ptr<QSqlDatabase> db_;

    std::list<std::pair<QString, QString>>   recent_locations_;
    std::map<QString, decltype(recent_locations_)::iterator>  recent_mapping_;

};
