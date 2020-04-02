#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <chrono>
#include <iostream>
#include <QDesktopServices>
#include <QUrl>

using namespace std::literals;

Widget::Widget(QWidget* parent) :
    QWidget(parent)
{
    ui = new Ui::Widget;
    ui->setupUi(this);

    setUpQuickAccessPanel();
    setUpDirContentPanel();
}

Widget::~Widget()
{
}

void Widget::dir_selection_changed(const QItemSelection& selected, const QItemSelection&)
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

void Widget::on_dir_content_doubleClicked(const QModelIndex& index)
{
    QString file_name = file_system->fileName(index);

    if (file_system->isDir(index))
    {
        ui->dir_content->setRootIndex(index);

        ui->path_line->addItem(path_line_prefix + file_name);

        path.push_back(index);
    }
    else
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(file_system->filePath(index)));
    }

    // clear selection
    ui->dir_content->setCurrentIndex(QModelIndex());
}

void Widget::on_back_clicked()
{
    QModelIndex current_dir_index = ui->dir_content->rootIndex();
    QModelIndex parent_dir_index = current_dir_index.parent();

    ui->dir_content->setRootIndex(parent_dir_index);

    if (ui->path_line->count() > 0)
    {
        QListWidgetItem* last_item = ui->path_line->takeItem(ui->path_line->count() - 1);

        delete last_item;

        path.pop_back();
    }

    // clear selection
    ui->dir_content->setCurrentIndex(QModelIndex());
}

void Widget::on_forward_clicked()
{
    QModelIndex selected_index = ui->dir_content->currentIndex();

    if (selected_index.isValid())
    {
        QString file_name = file_system->fileName(selected_index);

        if (file_system->isDir(selected_index))
        {
            ui->dir_content->setRootIndex(selected_index);

            ui->path_line->addItem(path_line_prefix + file_name);

            path.push_back(selected_index);
        }
    }

     // clear selection
    ui->dir_content->setCurrentIndex(QModelIndex());
}

void Widget::on_path_line_itemClicked(QListWidgetItem*)
{
    auto current_row = ui->path_line->currentRow();
    QModelIndex new_index = path[current_row];

    ui->dir_content->setRootIndex(new_index);

    path.erase(path.begin() + current_row + 1, path.end());

    while (ui->path_line->count() - 1 != current_row)
    {
        QListWidgetItem* end_item = ui->path_line->takeItem(ui->path_line->count() - 1);
        delete end_item;
    }

    ui->path_line->clearSelection();
}

void Widget::setUpQuickAccessPanel()
{
    QTreeWidgetItem* quick_access = addCategory(ui->quick_panel, QUICK_ACCESS_SECTION_NAME);
    setUpQuickAccess(quick_access);

    QTreeWidgetItem* recent = addCategory(ui->quick_panel, RECENT_SECTION_NAME);
    setUpRecent(recent);

    quick_access->setExpanded(true);
    recent->setExpanded(true);
}

void Widget::on_quick_panel_itemDoubleClicked(QTreeWidgetItem* item, int column)
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
        auto it = recent_mapping[item->text(column)];
        recent_locations.emplace_front(*it);
        recent_locations.erase(it);
        recent_mapping[item->text(column)] = recent_locations.begin();
        updateRecentSection();
        new_path = recent_locations.front().second;
    }
    else
    {
        return;
    }

    QModelIndex new_path_index = file_system->index(new_path);

    ui->dir_content->setRootIndex(new_path_index);

    path.clear();
    ui->path_line->clear();
    while (new_path_index.isValid())
    {
        path.push_front(new_path_index);
        ui->path_line->insertItem(0, path_line_prefix + file_system->fileName(new_path_index));
        new_path_index = new_path_index.parent();
    }

}

QTreeWidgetItem* Widget::addCategory(QTreeWidget* parent, QString name)
{
    QTreeWidgetItem* new_category = new QTreeWidgetItem(parent);
    new_category->setText(0, std::move(name));
    ui->quick_panel->addTopLevelItem(new_category);

    return new_category;
}

QTreeWidgetItem* Widget::addChild(QTreeWidgetItem* parent, QString name)
{
    QTreeWidgetItem* new_category = new QTreeWidgetItem(parent);
    new_category->setText(0, std::move(name));

    return new_category;
}

void Widget::setUpQuickAccess(QTreeWidgetItem* quick_access)
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

void Widget::setUpRecent(QTreeWidgetItem* recent)
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
    auto it = records.rbegin();
    auto beg = records.rbegin();
    auto end = records.rend();
    for (; it != end && std::distance(beg, it) < MAX_RECENT; ++it)
    {
        recent_locations.emplace_front(*it);
        recent_mapping.emplace(it->first, recent_locations.begin());
    }

    // update ui
    updateRecentSection();
}

void Widget::updateQuickAccessSection()
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

void Widget::updateRecentSection()
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

void Widget::setUpDirContentPanel()
{
    file_system = std::make_unique<QFileSystemModel>(this);
    ui->dir_content->setModel(file_system.get());
    ui->dir_content->setRootIndex(file_system->setRootPath(""));
    ui->dir_content->setItemsExpandable(false);

    // hide size, type, date modified,
    ui->dir_content->hideColumn(1);
    ui->dir_content->hideColumn(2);
    ui->dir_content->hideColumn(3);

    QObject::connect(ui->dir_content->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &Widget::dir_selection_changed);

    ui->dir_content->header()->setStyleSheet(QString::fromUtf8("border: 1px solid #FFFFFF;"
                                                               "border-bottom: 1px solid #D3D3D3"));
}
