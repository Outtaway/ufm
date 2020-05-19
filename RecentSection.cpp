#include "RecentSection.h"
#include "Settings.h"

#include <QTreeWidget>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

RecentSection::RecentSection(QTreeWidgetItem* section, QString name) : SectionBase(section, name)
{
    setup();
}

RecentSection::~RecentSection()
{
    exportRecentsToDatabase_();
}

void RecentSection::setup()
{
    if (!Settings::getInstance().optionExist(Settings::OPTIONS::MAX_RECENT))
    {
        const int INIT_MAX_RECENT = 5;
        Settings::getInstance().setOption(Settings::OPTIONS::MAX_RECENT, INIT_MAX_RECENT);
    }

    db_ = std::make_unique<QSqlDatabase>();

    if (!openDatabase_())
        return;

    if (!importRecentsFromDatabase_())
        return;

    QIcon icon(":/resources/recent.png");
    section_->setIcon(0, icon);

    SectionBase::setup();
    updateUi();
}

void RecentSection::addItem(QString name, QString path)
{
    if (recentExists_(name))
    {
        moveRecentToTop_(name);
        return;
    }

    recent_locations_.emplace_front(name, path);
    recent_mapping_[name] = recent_locations_.begin();

    auto MAX_RECENT = Settings::getInstance().getOption(Settings::OPTIONS::MAX_RECENT).toUInt();
    if (recent_locations_.size() > MAX_RECENT)
    {
        recent_mapping_.erase(recent_locations_.back().first);
        recent_locations_.pop_back();
    }
}

void RecentSection::updateUi()
{
    SectionBase::clearSection_();

    for (const auto& recent_location : recent_locations_)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem(section_);
        QIcon icon(":/resources/folder.png");
        child->setIcon(0, icon);
        child->setText(0, recent_location.first);

        section_->addChild(child);
    }
}

QString RecentSection::getPathByName(QString name)
{
    return recent_mapping_[name]->second;
}

void RecentSection::moveRecentToTop_(QString name)
{
    if (auto it = recent_mapping_.find(name); it != recent_mapping_.end())
    {
        recent_locations_.emplace_front(*(it->second));
        recent_locations_.erase(it->second);
        recent_mapping_[name] = recent_locations_.begin();
    }
    else
    {
        qDebug() << "Couldn't move " << name << " to the top. It does not exist";
    }
}

bool RecentSection::recentExists_(QString name)
{
    return recent_mapping_.find(name) != recent_mapping_.end();
}

bool RecentSection::openDatabase_()
{
    *db_ = QSqlDatabase::addDatabase("QSQLITE");

    db_->setDatabaseName(RECENT_DB_NAME);

    if (!db_->open())
    {
        qDebug() << "Database was not opened: " << RECENT_DB_NAME << ", reason: " << db_->lastError().text();
        return false;
    }

    return true;
}

bool RecentSection::importRecentsFromDatabase_()
{
    QSqlQuery query(*db_);

    QString create_table_query = "CREATE TABLE recent(\"name\" TEXT, \"path\" TEXT);";

    if (!query.exec(create_table_query))
    {
        qDebug() << query.lastError().databaseText();
    }

    QString select_all_query = "SELECT * FROM recent;";

    if (!query.exec(select_all_query))
    {
        qDebug() << "Couldn't execute query: " << select_all_query << ". Reason: " << query.lastError().databaseText();
        return false;
    }

    // read out database
    std::vector<std::pair<QString, QString>> records;
    while (query.next())
    {
        QString folder_name = query.value(RECENT_TABLE_COLUMNS::NAME).toString();
        QString path_name = query.value(RECENT_TABLE_COLUMNS::PATH).toString();

        records.emplace_back(std::move(folder_name), std::move(path_name));
    }

    // fill internal datastructures
    auto it = records.begin();
    auto beg = records.begin();
    auto end = records.end();
    auto MAX_RECENT = Settings::getInstance().getOption(Settings::OPTIONS::MAX_RECENT).toUInt();
    for (; it != end && std::distance(beg, it) < MAX_RECENT; ++it)
    {
        recent_locations_.emplace_front(*it);
        recent_mapping_.emplace(it->first, recent_locations_.begin());
    }
    return true;
}

bool RecentSection::exportRecentsToDatabase_()
{
    // clear what is inside recent table
    QSqlQuery query(*db_);

    QString clear_table_query = "DELETE FROM ";
    clear_table_query += RECENT_TABLE_NAME;
    clear_table_query += ";";

    if (!query.exec(clear_table_query))
    {
        qDebug() << "Couldn't clear table. Reason: " << query.lastError().databaseText();
        return false;
    }

    // insert what is in recent section to table
    QString insert_query = "INSERT INTO ";
    insert_query += RECENT_TABLE_NAME;
    insert_query += " VALUES(\"name\", \"path\");";

    for (auto it = recent_locations_.rbegin(); it != recent_locations_.rend(); ++it)
    {
        QString tmp_query = insert_query;
        tmp_query.replace("name", it->first);
        tmp_query.replace("path", it->second);

        qDebug() << "Executing query: " << tmp_query;

        if (!query.exec(tmp_query))
        {
            qDebug() << "Couldn't insert row in table. " << query.lastError().databaseText();
            return false;
        }
    }

    return true;
}
