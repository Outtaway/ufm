#ifndef WIDGET_H
#define WIDGET_H

#include <thread>
#include <QWidget>
#include <QFileSystemModel>

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

private:
    Ui::Widget* ui;
    QFileSystemModel* file_system;
    std::thread debug_thread_;

};
#endif // WIDGET_H
