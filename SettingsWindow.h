#pragma once

#include <QWidget>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void on_delete_aproval_check_stateChanged(int);

    void on_recent_entries_box_valueChanged(int new_value);

private:
    Ui::SettingsWindow *ui;
};

