#pragma once

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
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "Content.h"
#include "ContentContextMenu.h"
#include "PathLine.h"
#include "QuickPanel.h"
#include "MenuBar.h"
#include "SettingsButton.h"

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

    void on_dir_content_custom_menu(const QPoint& point);

    void on_back_clicked();

    void on_forward_clicked();

    void on_path_line_itemClicked(QListWidgetItem* item);

    void on_quick_panel_itemDoubleClicked(QTreeWidgetItem* item, int column);

    void onExit();

private:

    Ui::Window*                         ui;

    ContentContextMenu* content_context_menu_;

    QHBoxLayout* layout_;
    MenuBar* menu_bar_;
    SettingsButton* settings_button_;

    std::shared_ptr<Content>            content_;
    std::unique_ptr<PathLine>           path_line_;
    std::unique_ptr<QuickPanel>         quick_panel_;

};
