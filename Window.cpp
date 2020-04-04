#include "Window.h"
#include "ui_window.h"
#include <QDebug>
#include <chrono>
#include <iostream>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>

using namespace std::literals;

Window::Window(QWidget* parent) :
    QWidget(parent)
{
    ui = new Ui::Window;
    ui->setupUi(this);

    content_ = std::make_unique<Content>(ui->dir_content);
    content_->setup();

    path_line_ = std::make_unique<PathLine>(ui->path_line);

    setUpQuickAccessPanel();

    QObject::connect(QApplication::instance(), &QApplication::aboutToQuit, this, &Window::onExit);

    QObject::connect(ui->dir_content->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &Window::dir_selection_changed);
}

Window::~Window()
{
}

void Window::dir_selection_changed(const QItemSelection& selected, const QItemSelection&)
{
    if (!selected.indexes().empty())
    {
        ui->forward->setEnabled(true);
    }
    else
    {
        ui->forward->setEnabled(false);
    }
}

void Window::on_dir_content_doubleClicked(const QModelIndex& index)
{
    if (content_->isDirectory(index))
    {
        content_->setCurrentDirectory(index);

        path_line_->pushBack(content_->getCurrentDirectory(), content_->getCurrentDirectoryName());

        if (recentExists(content_->getCurrentDirectoryName()))
        {
            moveRecentToTop(content_->getCurrentDirectoryName());
        }
        else
        {
            addRecent(content_->getCurrentDirectoryName(), content_->getCurrentDirectoryFullPath());
        }

        updateRecentSection();
    }
    else
        content_->tryExecute(index);

    content_->clearSelection();
}

void Window::on_back_clicked()
{
    if (content_->goOneDirectoryBack())
        path_line_->popBack();

    content_->clearSelection();
}

void Window::on_forward_clicked()
{
    // TODO: fix bad behavior when clicked back on root folder("")
    if (content_->goOneDirectoryForward())
        path_line_->pushBack(content_->getCurrentDirectory(), content_->getCurrentDirectoryName());

    content_->clearSelection();
}

void Window::on_path_line_itemClicked(QListWidgetItem*)
{
    auto selected_directory_pos = path_line_->selectedDirectoryPos();

    QModelIndex new_directory = path_line_->directoryIndexAt(selected_directory_pos);
    content_->setCurrentDirectory(new_directory);

    path_line_->erase(selected_directory_pos + 1u, path_line_->size() - (selected_directory_pos + 1u));

    path_line_->clearSelection();
}

void Window::setUpQuickAccessPanel()
{
    QTreeWidgetItem* quick_access = addCategory(ui->quick_panel, QUICK_ACCESS_SECTION_NAME);
    setUpQuickAccess(quick_access);

    QTreeWidgetItem* recent = addCategory(ui->quick_panel, RECENT_SECTION_NAME);
    setUpRecent(recent);

    quick_access->setExpanded(true);
    recent->setExpanded(true);
}

void Window::on_quick_panel_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    QTreeWidgetItem* parent = item->parent();

    if (!parent)
    {
        return;
    }

    QString new_path;

    if (parent->text(0) == QUICK_ACCESS_SECTION_NAME)
    {
        new_path = standart_locations[item->text(column)];
    }
    else if (parent->text(0) == RECENT_SECTION_NAME)
    {
        moveRecentToTop(item->text(column));
        updateRecentSection();
        new_path = recent_locations.front().second;
    }
    else
    {
        return;
    }

    content_->setCurrentDirectoryString(new_path);

    path_line_->setPathChain(content_->composeCurrentDirPathChain());
}

void Window::onExit()
{
    exportRecentToDatabase();
}

QTreeWidgetItem* Window::addCategory(QTreeWidget* parent, QString name)
{
    QTreeWidgetItem* new_category = new QTreeWidgetItem(parent);
    new_category->setText(0, std::move(name));
    ui->quick_panel->addTopLevelItem(new_category);

    return new_category;
}

QTreeWidgetItem* Window::addChild(QTreeWidgetItem* parent, QString name)
{
    QTreeWidgetItem* new_category = new QTreeWidgetItem(parent);
    new_category->setText(0, std::move(name));

    return new_category;
}

void Window::setUpQuickAccess(QTreeWidgetItem* quick_access)
{
    quick_access_section = quick_access;

    std::map<QString, QStringList> standart_dirs;

    standart_dirs.insert({ DESKTOP, QStandardPaths::standardLocations(QStandardPaths::DesktopLocation) });
    standart_dirs.insert({ DOCUMENTS, QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation) });
    standart_dirs.insert({ DOWNLOADS, QStandardPaths::standardLocations(QStandardPaths::DownloadLocation) });
    standart_dirs.insert({ MUSIC, QStandardPaths::standardLocations(QStandardPaths::MusicLocation) });
    standart_dirs.insert({ PICTURES, QStandardPaths::standardLocations(QStandardPaths::PicturesLocation) });
    standart_dirs.insert({ MOVIES, QStandardPaths::standardLocations(QStandardPaths::MoviesLocation) });

    // fill internal data structure
    for (auto& standart_dir : standart_dirs)
    {
        if (!standart_dir.second.empty())
        {
            standart_locations.emplace(standart_dir.first, standart_dir.second.last());
        }
    }

    // update ui
    updateQuickAccessSection();
}

void Window::setUpRecent(QTreeWidgetItem* recent)
{
    recent_section = recent;

    db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName(RECENT_DB_NAME);

    if (!db.open())
    {
        qDebug() << "Database was not opened: " << RECENT_DB_NAME << ", reason: " << db.lastError().text();
        return;
    }

    QSqlQuery query(db);

    QString create_table_query = "CREATE TABLE recent(\"name\" TEXT, \"path\" TEXT);";

    if (!query.exec(create_table_query))
    {
        qDebug() << query.lastError().databaseText();
    }

    QString select_all_query = "SELECT * FROM recent;";

    if (!query.exec(select_all_query))
    {
        qDebug() << "Couldn't execute query: " << select_all_query << ". Reason: " << query.lastError().databaseText();
        return;
    }

    // read out database
   
    std::vector<std::pair<QString, QString>> records;
    
    while (query.next())
    {
        QString folder_name = query.value(RECENT_TABLE_COLUMNS::NAME).toString();
        QString path_name = query.value(RECENT_TABLE_COLUMNS::PATH).toString();

        records.emplace_back(folder_name, path_name);
    }

    // fill internal datastructures
    auto it = records.begin();
    auto beg = records.begin();
    auto end = records.end();
    for (; it != end && std::distance(beg, it) < MAX_RECENT; ++it)
    {
        recent_locations.emplace_front(*it);
        recent_mapping.emplace(it->first, recent_locations.begin());
    }

    // update ui
    updateRecentSection();
}

void Window::updateQuickAccessSection()
{
    auto children = quick_access_section->takeChildren();

    for (auto& child : children)
    {
        delete child;
    }

    for (const auto& standart_location : standart_locations)
    {
        addChild(quick_access_section, standart_location.first);
    }
}

void Window::updateRecentSection()
{
    auto children = recent_section->takeChildren();

    for (auto& child : children)
    {
        delete child;
    }

    for (const auto& recent_location : recent_locations)
    {
        addChild(recent_section, recent_location.first);
    }
}

bool Window::recentExists(QString file_name)
{
    return recent_mapping.find(file_name) != recent_mapping.end();
}

void Window::moveRecentToTop(QString file_name)
{
    if (recent_mapping.find(file_name) != recent_mapping.end())
    {
        auto it = recent_mapping[file_name];
        recent_locations.emplace_front(*it);
        recent_locations.erase(it);
        recent_mapping[file_name] = recent_locations.begin();
    }
    else
    {
        qDebug() << "Couldn't move " << file_name << " to the top. It does not exist";
    }
}

void Window::addRecent(QString file_name, QString file_path)
{
    recent_locations.emplace_front(file_name, file_path);
    recent_mapping[file_name] = recent_locations.begin();

    if (recent_locations.size() > MAX_RECENT)
    {
        recent_mapping.erase(recent_locations.back().first);
        recent_locations.pop_back();
    }
}

void Window::exportRecentToDatabase()
{
    // clear what is inside recent table
    QSqlQuery query(db);

    QString clear_table_query = "DELETE FROM ";
    clear_table_query += RECENT_TABLE_NAME;
    clear_table_query += ";";

    if (!query.exec(clear_table_query))
    {
        qDebug() << "Couldn't clear table. Reason: " << query.lastError().databaseText();
        return;
    }

    // insert what is in recent section to table

    QString insert_query = "INSERT INTO recent VALUES(";

    for (auto it = recent_locations.rbegin(); it != recent_locations.rend(); ++it)
    {
        QString name = "\"" + it->first + "\"";
        QString path = "\"" + it->second + "\"";
        QString tmp_query = insert_query + name + "," + path + ");";

        qDebug() << "Executing query: " << tmp_query;

        if (!query.exec(tmp_query))
        {
            qDebug() << "Couldn't insert row in table. " << query.lastError().databaseText();
            return;
        }
    }
}
