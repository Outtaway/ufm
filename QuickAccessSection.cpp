#include "QuickAccessSection.h"

#include <QTreeWidget>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>

QuickAccessSection::QuickAccessSection(QTreeWidgetItem* section, QString name) : SectionBase(section, name)
{
    setup();
}

QuickAccessSection::~QuickAccessSection()
{
    exportQuickAccessToDatabase_();
}

void QuickAccessSection::setup()
{
    standart_locations_.emplace(DESKTOP, QStandardPaths::standardLocations(QStandardPaths::DesktopLocation));
    standart_locations_.emplace(DOCUMENTS, QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
    standart_locations_.emplace(DOWNLOADS, QStandardPaths::standardLocations(QStandardPaths::DownloadLocation));
    standart_locations_.emplace(MUSIC, QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    standart_locations_.emplace(PICTURES, QStandardPaths::standardLocations(QStandardPaths::PicturesLocation));
    standart_locations_.emplace(MOVIES, QStandardPaths::standardLocations(QStandardPaths::MoviesLocation));

    locations_png_.emplace(DESKTOP, ":/resources/desktop.png");
    locations_png_.emplace(DOCUMENTS, ":/resources/documents.png");
    locations_png_.emplace(DOWNLOADS, ":/resources/downloads.png");
    locations_png_.emplace(MUSIC, ":/resources/music.png");
    locations_png_.emplace(PICTURES, ":/resources/pictures.png");
    locations_png_.emplace(MOVIES, ":/resources/movies.png");

    // fill internal data structure
    for (auto& standart_dir : standart_locations_)
    {
        if (!standart_dir.second.empty())
        {
            addItem(standart_dir.first, standart_dir.second.last());
        }
    }

    db_ = std::make_unique<QSqlDatabase>();

    SectionBase::setup();

    QIcon icon(":/resources/quick_access.png");
    section_->setIcon(0, icon);

    if (!openDatabase_())
        return;

    if (!importQuickAccessFromDatabase_())
        return;

    updateUi();
}

void QuickAccessSection::addItem(QString name, QString path)
{
    quick_items_.emplace(std::move(name), std::move(path));
}

void QuickAccessSection::deleteItem(QString name)
{
    if (!isStandartLocation_(name))
        quick_items_.erase(name);
}

void QuickAccessSection::updateUi()
{
    SectionBase::clearSection_();

    for (const auto& standart_location : quick_items_)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem(section_);

        if (locations_png_.find(standart_location.first) != locations_png_.end())
        {
            child->setIcon(0, locations_png_[standart_location.first]);
        }
        else
        {
            // if item is not standart location, then just set folder icon
            QIcon icon(":/resources/folder.png");
            child->setIcon(0, icon);
        }

        child->setText(0, standart_location.first);

        section_->addChild(child);
    }
}

QString QuickAccessSection::getPathByName(QString name)
{
    return quick_items_[name];
}

bool QuickAccessSection::importQuickAccessFromDatabase_()
{
    QSqlQuery query(*db_);

    QString create_table_query = "CREATE TABLE quick_access(\"name\" TEXT, \"path\" TEXT);";

    if (!query.exec(create_table_query))
    {
        qDebug() << query.lastError().databaseText();
    }

    QString select_all_query = "SELECT * FROM quick_access;";

    if (!query.exec(select_all_query))
    {
        qDebug() << "Couldn't execute query: " << select_all_query << ". Reason: " << query.lastError().databaseText();
        return false;
    }

    // read out database
    while (query.next())
    {
        QString folder_name = query.value(QUICK_ACCESS_TABLE_COLUMNS::NAME).toString();
        QString path = query.value(QUICK_ACCESS_TABLE_COLUMNS::PATH).toString();
        addItem(std::move(folder_name), std::move(path));
    }

    return true;
}

bool QuickAccessSection::exportQuickAccessToDatabase_()
{
    // clear what is inside quick access table
    QSqlQuery query(*db_);

    QString clear_table_query = "DELETE FROM ";
    clear_table_query += QUICK_ACCESS_TABLE_NAME;
    clear_table_query += ";";

    if (!query.exec(clear_table_query))
    {
        qDebug() << "Couldn't clear table. Reason: " << query.lastError().databaseText();
        return false;
    }

    // insert what is in quick access section to table
    QString insert_query = "INSERT INTO ";
    insert_query += QUICK_ACCESS_TABLE_NAME;
    insert_query += " VALUES(\"name\", \"path\");";

    for (auto it = quick_items_.begin(); it != quick_items_.end(); ++it)
    {
        if (!isStandartLocation_(it->first))
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
    }

    return true;
}

bool QuickAccessSection::openDatabase_()
{
    *db_ = QSqlDatabase::addDatabase("QSQLITE", "quick_access_connection");

    db_->setDatabaseName(QUICK_ACCESS_DB_NAME);

    if (!db_->open())
    {
        qDebug() << "Database was not opened: " << QUICK_ACCESS_DB_NAME << ", reason: " << db_->lastError().text();
        return false;
    }

    return true;
}

bool QuickAccessSection::isStandartLocation_(QString name)
{
    return standart_locations_.find(name) != standart_locations_.end();
}
