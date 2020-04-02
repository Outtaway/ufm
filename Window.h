#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include <deque>
#include <map>

#include <QWidget>
#include <QFileSystemModel>
#include <QItemSelection>
#include <QListWidget>
#include <QTreeWidget>
#include <QtSql>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:

    Window(QWidget *parent = nullptr);
    ~Window();

private slots:

    void dir_selection_changed(const QItemSelection& prev, const QItemSelection& next);

    void on_dir_content_doubleClicked(const QModelIndex& index);

    void on_back_clicked();

    void on_forward_clicked();

    void on_path_line_itemClicked(QListWidgetItem* item);

    void setUpQuickAccessPanel();

    void setUpDirContentPanel();

    void on_quick_panel_itemDoubleClicked(QTreeWidgetItem* item, int column);

    void onExit();
private:

    Ui::Window*                         ui;
    std::unique_ptr<QFileSystemModel>   file_system;

    std::deque<QModelIndex>             path;
    const QString                       path_line_prefix = " 🠚 ";

    QTreeWidgetItem* addCategory(QTreeWidget* parent, QString name);
    QTreeWidgetItem* addChild(QTreeWidgetItem* parent, QString name);

    // quick access related
    const char* const QUICK_ACCESS_SECTION_NAME = "Quick access";
    const char* const DESKTOP = "Desktop";
    const char* const DOCUMENTS = "Documents";
    const char* const DOWNLOADS = "Downloads";
    const char* const MUSIC = "Music";
    const char* const PICTURES = "Pictures";
    const char* const MOVIES = "Movies";
    QTreeWidgetItem* quick_access_section;
    void setUpQuickAccess(QTreeWidgetItem* quick_access);
    void updateQuickAccessSection();
    std::map<QString, QString>          standart_locations;

    // recent panel related
    enum RECENT_TABLE_COLUMNS
    {
        NAME = 0,
        PATH
    };
    const char* const RECENT_SECTION_NAME = "Recent";
    const char* const RECENT_DB_NAME = "recent.db";
    const char* const RECENT_TABLE_NAME = "recent";
    QSqlDatabase db;
    std::list<std::pair<QString, QString>>          recent_locations;
    std::map<QString, decltype(recent_locations)::iterator> recent_mapping;
    const int MAX_RECENT = 5;
    QTreeWidgetItem* recent_section;
    void setUpRecent(QTreeWidgetItem* recent);
    void updateRecentSection();
    bool recentExists(QString file_name);
    void moveRecentToTop(QString file_name);
    void addRecent(QString file_name, QString file_path);
    void exportRecentToDatabase();
};

#endif // WINDOW_h
