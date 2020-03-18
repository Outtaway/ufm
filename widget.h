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

    void on_dir_content_doubleClicked(const QModelIndex &index);

    void on_back_clicked();

    void on_forward_clicked();

private:
    Ui::Widget* ui;
    QFileSystemModel* file_system;

};

#endif // WIDGET_H
