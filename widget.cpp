#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <chrono>
#include <iostream>

using namespace std::literals;

Widget::Widget(QWidget* parent) :
    QWidget(parent)
{
    ui = new Ui::Widget;
    ui->setupUi(this);

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
        qDebug() << file_name<< " is not a directory";
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
