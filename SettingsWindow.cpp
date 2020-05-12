#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "Settings.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);

    ui->recent_entries_box->setRange(0, 20);
    ui->delete_aproval_check->setChecked(Settings::getOption(Settings::OPTIONS::DELETE_APPROVAL).toBool());
    ui->recent_entries_box->setValue(Settings::getOption(Settings::OPTIONS::MAX_RECENT).toUInt());
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_delete_aproval_check_stateChanged(int)
{
    Settings::setOption(Settings::OPTIONS::DELETE_APPROVAL, ui->delete_aproval_check->isChecked());
}

void SettingsWindow::on_recent_entries_box_valueChanged(int new_value)
{
    Settings::setOption(Settings::OPTIONS::MAX_RECENT, new_value);
}
