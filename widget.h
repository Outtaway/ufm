#ifndef WIDGET_H
#define WIDGET_H

#include <memory>
#include <deque>

#include <QWidget>
#include <QFileSystemModel>
#include <QItemSelection>
#include <QListWidget>

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

private:

    Ui::Widget*                         ui;
    std::unique_ptr<QFileSystemModel>   file_system;

    std::deque<QModelIndex>             path;
    const QString                       path_line_prefix = " 🠚 ";
};

#endif // WIDGET_H
