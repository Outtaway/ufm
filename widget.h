#ifndef WIDGET_H
#define WIDGET_H

#include <memory>
#include <deque>
#include <map>

#include <QWidget>
#include <QFileSystemModel>
#include <QItemSelection>
#include <QListWidget>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:

    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void dir_selection_changed(const QItemSelection& prev, const QItemSelection& next);

    void on_dir_content_doubleClicked(const QModelIndex& index);

    void on_back_clicked();

    void on_forward_clicked();

    void on_path_line_itemClicked(QListWidgetItem* item);

    void setUpQuickAccessPanel();

    void setUpDirContentPanel();

    void on_quick_panel_itemDoubleClicked(QTreeWidgetItem* item, int column);

private:

    Ui::Widget*                         ui;
    std::unique_ptr<QFileSystemModel>   file_system;

    std::deque<QModelIndex>             path;
    const QString                       path_line_prefix = " 🠚 ";

    // quick panel related
    const char* const DESKTOP = "Desktop";
    const char* const DOCUMENTS = "Documents";
    const char* const DOWNLOADS = "Downloads";
    const char* const MUSIC = "Music";
    const char* const PICTURES = "Pictures";
    const char* const MOVIES = "Movies";
    QTreeWidgetItem* addCategory(QTreeWidget* parent, QString name);
    QTreeWidgetItem* addChild(QTreeWidgetItem* parent, QString name);
    void setUpQuickAccess(QTreeWidgetItem* quick_access);
    void setUpRecent(QTreeWidgetItem* recent);
    std::map<QString, QString>          standart_locations;
};

#endif // WIDGET_H
