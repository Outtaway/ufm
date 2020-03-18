#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <chrono>

using namespace std::literals;

Widget::Widget(QWidget *parent) :
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

void Widget::on_dir_content_doubleClicked(const QModelIndex &index)
{
    auto entity_info = file_system->fileInfo(index);

    if (entity_info.isDir())
    {
        ui->path_line->setText(entity_info.path());
    }
    else
    {
        qDebug() << entity_info.path() << " is not a directory";
    }
}
