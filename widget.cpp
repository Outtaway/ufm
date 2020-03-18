#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <chrono>

using namespace std::literals;

Widget::Widget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    file_system = new QFileSystemModel(this);
    ui->dir_content->setModel(file_system);
    ui->dir_content->setRootIndex(file_system->setRootPath(""));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_dir_content_doubleClicked(const QModelIndex& index)
{
    QFileInfo entity_info = file_system->fileInfo(index);

    if (entity_info.isDir())
    {
        ui->dir_content->setRootIndex(index);

        ui->path_line->setText(entity_info.absoluteFilePath());
    }
    else
    {
        qDebug() << entity_info.path() << " is not a directory";
    }
}

void Widget::on_back_clicked()
{
    QModelIndex current_dir_index = ui->dir_content->rootIndex();
    QModelIndex parent_dir_index = current_dir_index.parent();
    QFileInfo   parent_dir_info = file_system->fileInfo(parent_dir_index);

    ui->dir_content->setRootIndex(parent_dir_index);
    ui->path_line->setText(parent_dir_info.absoluteFilePath());
    ui->dir_content->setCurrentIndex(QModelIndex());
}

void Widget::on_forward_clicked()
{
    QModelIndex selected_index = ui->dir_content->currentIndex();

    if (selected_index.isValid())
    {
        QFileInfo entity_info = file_system->fileInfo(selected_index);

        if (entity_info.isDir())
        {
            ui->dir_content->setRootIndex(selected_index);

            ui->path_line->setText(entity_info.absoluteFilePath());
        }
    }

    ui->dir_content->setCurrentIndex(QModelIndex());
}
