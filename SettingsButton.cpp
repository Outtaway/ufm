#include "SettingsButton.h"

#include <QIcon>

#include "SettingsWindow.h"

SettingsButton::SettingsButton(QWidget* parent) :
    QPushButton(parent)
{
    QIcon settings_icon(":/resources/settings.png");

    this->setIcon(settings_icon);
    this->setIconSize(QSize(20, 20));
    this->setStyleSheet("QPushButton:hover { border: 1px solid #D3D3D3; }");
    this->setFlat(true);

    QObject::connect(this, &QPushButton::clicked, this, &SettingsButton::settingsInvoked);
}

void SettingsButton::settingsInvoked()
{
    SettingsWindow* settings_window = new SettingsWindow;
    settings_window->setWindowModality(Qt::ApplicationModal);
    settings_window->setWindowTitle("UFM Settings");
    settings_window->setAttribute(Qt::WA_DeleteOnClose);
    settings_window->show();
}
