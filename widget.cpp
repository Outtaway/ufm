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
    ui->dir_content->setRootIndex(file_system->setRootPath(":/"));
    ui->dir_content->hideColumn(1);
    ui->dir_content->hideColumn(2);
    ui->dir_content->hideColumn(3);
    ui->dir_content->setHeaderHidden(true);

}

Widget::~Widget()
{
    delete ui;
}

