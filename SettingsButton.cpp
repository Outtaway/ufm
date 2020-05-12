#include "SettingsButton.h"

#include <QIcon>

#include <QDebug>

#include "SettingsWindow.h"

SettingsButton::SettingsButton(QWidget* parent) :
    QPushButton(parent)
{
    QIcon settings_icon(":/resources/settings.png");

    this->setIcon(settings_icon);
    this->setIconSize(QSize(20, 20));

    QObject::connect(this, &QPushButton::clicked, this, &SettingsButton::settingsInvoked);
}

void SettingsButton::settingsInvoked()
{
    SettingsWindow* settings_window = new SettingsWindow;
    settings_window->setWindowTitle("UFM Settings");
    settings_window->setAttribute(Qt::WA_DeleteOnClose);
    settings_window->show();
}
