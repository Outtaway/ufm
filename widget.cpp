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

    ui->treeView->setModel(file_system);

    ui->treeView->setRootIndex(file_system->setRootPath("C:/"));

    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
}

Widget::~Widget()
{
    delete ui;
}

