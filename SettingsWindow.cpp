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
    ui->delete_aproval_check->setChecked(Settings::Settings::getInstance().getOption(Settings::OPTIONS::DELETE_APPROVAL).toBool());
    ui->recent_entries_box->setValue(Settings::getInstance().getOption(Settings::OPTIONS::MAX_RECENT).toUInt());
    ui->show_size_check->setChecked(Settings::Settings::getInstance().getOption(Settings::OPTIONS::SHOW_SIZE).toBool());
    ui->show_types_check->setChecked(Settings::Settings::getInstance().getOption(Settings::OPTIONS::SHOW_TYPE).toBool());
    ui->show_last_modified_check->setChecked(Settings::Settings::getInstance().getOption(Settings::OPTIONS::SHOW_LAST_MODIFIED).toBool());
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_delete_aproval_check_stateChanged(int)
{
    Settings::getInstance().setOption(Settings::OPTIONS::DELETE_APPROVAL, ui->delete_aproval_check->isChecked());
}

void SettingsWindow::on_recent_entries_box_valueChanged(int new_value)
{
    Settings::getInstance().setOption(Settings::OPTIONS::MAX_RECENT, new_value);
}

void SettingsWindow::on_show_last_modified_check_stateChanged(int)
{
    Settings::getInstance().setOption(Settings::OPTIONS::SHOW_LAST_MODIFIED, ui->show_last_modified_check->isChecked());
}

void SettingsWindow::on_show_size_check_stateChanged(int)
{
    Settings::getInstance().setOption(Settings::OPTIONS::SHOW_SIZE, ui->show_size_check->isChecked());
}

void SettingsWindow::on_show_types_check_stateChanged(int)
{
    Settings::getInstance().setOption(Settings::OPTIONS::SHOW_TYPE, ui->show_types_check->isChecked());
}
